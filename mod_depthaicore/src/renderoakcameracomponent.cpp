
// Local Includes
#include "renderoakcameracomponent.h"
#include "oakshader.h"

// External Includes
#include <entity.h>
#include <orthocameracomponent.h>
#include <nap/core.h>
#include <renderservice.h>
#include <renderglobals.h>
#include <glm/gtc/matrix_transform.hpp>


RTTI_BEGIN_CLASS(nap::RenderOakCameraComponent)
RTTI_PROPERTY("OAK frame",				&nap::RenderOakCameraComponent::mOakFrame,			nap::rtti::EPropertyMetaData::Required);
RTTI_PROPERTY("Rendering Texture",		&nap::RenderOakCameraComponent::mOutputTexture,		nap::rtti::EPropertyMetaData::Required);
RTTI_END_CLASS

RTTI_BEGIN_CLASS_NO_DEFAULT_CONSTRUCTOR(nap::RenderOakCameraComponentInstance)
RTTI_CONSTRUCTOR(nap::EntityInstance&, nap::Component&)
RTTI_END_CLASS




namespace nap
{
	static void computeModelMatrix(const nap::IRenderTarget& target, glm::mat4& outMatrix) 
	{
		// Transform to middle of target
		glm::ivec2 tex_size = target.getBufferSize();
		outMatrix = glm::translate(glm::mat4(), glm::vec3(
			tex_size.x / 2.0f,
			tex_size.y / 2.0f,
			0.0f));

		outMatrix = glm::scale(outMatrix, glm::vec3(tex_size.x, tex_size.y, 1.0f));

	}

	RenderOakCameraComponentInstance::RenderOakCameraComponentInstance(EntityInstance& entity, Component& resource) :RenderableComponentInstance(entity, resource),
		mTarget(*entity.getCore()), mPlane(*entity.getCore())
	{
	}

	bool RenderOakCameraComponentInstance::init(utility::ErrorState& errorState)
	{
		if (!RenderableComponentInstance::init(errorState))
			return false;

		RenderOakCameraComponent* resource = getComponent<RenderOakCameraComponent>();
		
		mOakFrame = resource->mOakFrame.get();
		mOutputTexture = resource->mOutputTexture.get();
		if (!errorState.check(mOutputTexture != nullptr, "%s: no output texture", resource->mID.c_str()))
			return false;
		if (!errorState.check(mOutputTexture->mFormat == RenderTexture2D::EFormat::RGBA8, "%s: output texture color format is not RGBA8", resource->mID.c_str()))
			return false;
		mTarget.mClearColor = resource->mClearColor.convert<RGBAColorFloat>();
		//mTarget.mClearColor = RGBAColor8(1., 1., 1., 1.);
		mTarget.mColorTexture = resource->mOutputTexture;
		mTarget.mSampleShading = true;
		mTarget.mRequestedSamples = resource->mRequestedSamples;
		if (!mTarget.init(errorState))
			return false;

		// Now create a plane and initialize it
		// The plane is positioned on update based on current texture output size
		mPlane.mSize = glm::vec2(1.0f, 1.0f);
		mPlane.mPosition = glm::vec3(0.0f, 0.0f, 0.0f);
		mPlane.mCullMode = ECullMode::Back;
		mPlane.mUsage = EMemoryUsage::Static;
		mPlane.mColumns = 1;
		mPlane.mRows = 1;

		if (!mPlane.init(errorState))
			return false;

		// Extract render service
		mRenderService = getEntityInstance()->getCore()->getService<RenderService>();
		assert(mRenderService != nullptr);


		// Get video material
		Material* oakMaterial = mRenderService->getOrCreateMaterial<OakShader>(errorState);
		if (!errorState.check(oakMaterial != nullptr, "%s: unable to get or create video material", resource->mID.c_str()))
			return false;

		// Create resource for the video material instance
		mMaterialInstanceResource.mBlendMode = EBlendMode::Opaque;
		mMaterialInstanceResource.mDepthMode = EDepthMode::NoReadWrite;
		mMaterialInstanceResource.mMaterial = oakMaterial;

		// Initialize oak material instance, used for rendering video
		if (!mMaterialInstance.init(*mRenderService, mMaterialInstanceResource, errorState))
			return false;

		// Ensure the mvp struct is available
		mMVPStruct = mMaterialInstance.getOrCreateUniform(uniform::mvpStruct);
		if (!errorState.check(mMVPStruct != nullptr, "%s: Unable to find uniform MVP struct: %s in material: %s",
			this->mID.c_str(), uniform::mvpStruct, mMaterialInstance.getMaterial().mID.c_str()))
			return false;

		// Get all matrices
		mModelMatrixUniform = ensureUniform(uniform::modelMatrix, errorState);
		mProjectMatrixUniform = ensureUniform(uniform::projectionMatrix, errorState);
		mViewMatrixUniform = ensureUniform(uniform::viewMatrix, errorState);

		if (mModelMatrixUniform == nullptr || mProjectMatrixUniform == nullptr || mViewMatrixUniform == nullptr)
			return false;

		// Get sampler inputs to update from video material
		mRGBASampler = ensureSampler(uniform::oakVideo::RGBASampler, errorState);
		mSemanticSegSampler = ensureSampler(uniform::oakVideo::SemanticSegSampler, errorState);

		if (mRGBASampler == nullptr || mSemanticSegSampler == nullptr)
			return false;


		// Create the renderable mesh, which represents a valid mesh / material combination
		mRenderableMesh = mRenderService->createRenderableMesh(mPlane, mMaterialInstance, errorState);
		if (!mRenderableMesh.isValid())
			return false;




		return true;
	}

	bool RenderOakCameraComponentInstance::isSupported(nap::CameraComponentInstance& camera) const
	{
		return camera.get_type().is_derived_from(RTTI_OF(OrthoCameraComponentInstance));
	}


	void RenderOakCameraComponentInstance::onDraw(IRenderTarget& renderTarget, VkCommandBuffer commandBuffer, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix)
	{
		// Update the model matrix so that the plane mesh is of the same size as the render target
		computeModelMatrix(renderTarget, mModelMatrix);
		mModelMatrixUniform->setValue(mModelMatrix);

		// Update matrices, projection and model are required
		mProjectMatrixUniform->setValue(projectionMatrix);
		mViewMatrixUniform->setValue(viewMatrix);

		// Get valid descriptor set
		const DescriptorSet& descriptor_set = mMaterialInstance.update();

		// Gather draw info
		MeshInstance& mesh_instance = mRenderableMesh.getMesh().getMeshInstance();
		GPUMesh& mesh = mesh_instance.getGPUMesh();

		// Get pipeline to to render with
		utility::ErrorState error_state;
		RenderService::Pipeline pipeline = mRenderService->getOrCreatePipeline(renderTarget, mRenderableMesh.getMesh(), mMaterialInstance, error_state);
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.mPipeline);
		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.mLayout, 0, 1, &descriptor_set.mSet, 0, nullptr);

		// Bind buffers and draw
		const std::vector<VkBuffer>& vertexBuffers = mRenderableMesh.getVertexBuffers();
		const std::vector<VkDeviceSize>& vertexBufferOffsets = mRenderableMesh.getVertexBufferOffsets();

		vkCmdBindVertexBuffers(commandBuffer, 0, vertexBuffers.size(), vertexBuffers.data(), vertexBufferOffsets.data());
		for (int index = 0; index < mesh_instance.getNumShapes(); ++index)
		{
			const IndexBuffer& index_buffer = mesh.getIndexBuffer(index);
			vkCmdBindIndexBuffer(commandBuffer, index_buffer.getBuffer(), 0, VK_INDEX_TYPE_UINT32);
			vkCmdDrawIndexed(commandBuffer, index_buffer.getCount(), 1, 0, 0, 0);
		}
	}
	
	UniformMat4Instance* RenderOakCameraComponentInstance::ensureUniform(const std::string& uniformName, utility::ErrorState& error)
	{
		assert(mMVPStruct != nullptr);
		UniformMat4Instance* found_uniform = mMVPStruct->getOrCreateUniform<UniformMat4Instance>(uniformName);
		if (!error.check(found_uniform != nullptr,
			"%s: unable to find uniform: %s in material: %s", this->mID.c_str(), uniformName.c_str(),
			mMaterialInstance.getMaterial().mID.c_str()))
			return nullptr;
		return found_uniform;
	}

	/**
	 * Checks if the sampler with the given name is available on the source material and creates it if so
	 * @return new or created sampler
	 */

	Sampler2DInstance* RenderOakCameraComponentInstance::ensureSampler(const std::string& samplerName, utility::ErrorState& error)
	{
		Sampler2DInstance* found_sampler = mMaterialInstance.getOrCreateSampler<Sampler2DInstance>(samplerName);
		if (!error.check(found_sampler != nullptr,
			"%s: unable to find sampler: %s in material: %s", this->mID.c_str(), samplerName.c_str(),
			mMaterialInstance.getMaterial().mID.c_str()))
			return nullptr;
		return found_sampler;
	}

	void RenderOakCameraComponentInstance::draw()
	{
		// Get current command buffer, should be headless.
		VkCommandBuffer command_buffer = mRenderService->getCurrentCommandBuffer();

		// Create orthographic projection matrix
		glm::ivec2 size = mTarget.getBufferSize();

		// Create projection matrix
		glm::mat4 proj_matrix = OrthoCameraComponentInstance::createRenderProjectionMatrix(0.0f, (float)size.x, 0.0f, (float)size.y);


		if (!hasBeenSet)
			hasBeenSet = setContentSampler();

		// Call on draw
		mTarget.beginRendering();
		onDraw(mTarget, command_buffer, glm::mat4(), proj_matrix);
		mTarget.endRendering();
	}




	bool RenderOakCameraComponentInstance::setContentSampler()
	{


		if (mOakFrame != nullptr) {

			if (mOakFrame->textureInit()) {
				mRGBASampler->setTexture(mOakFrame->getRGBATexture());
				mSemanticSegSampler->setTexture(mOakFrame->getSegmentationTexture());
				return true;
			}
			else {
				return false;
			}

		}
		return false;
	}


	
}