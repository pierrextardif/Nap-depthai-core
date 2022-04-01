
// Local Includes
#include "renderoakcameracomponent.h"
#include "camerashader.h"

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
		mPlane.mUsage = EMeshDataUsage::Static;
		mPlane.mColumns = 1;
		mPlane.mRows = 1;

		if (!mPlane.init(errorState))
			return false;

		// Extract render service
		mRenderService = getEntityInstance()->getCore()->getService<RenderService>();
		assert(mRenderService != nullptr);


		// Get video material
		Material* video_material = mRenderService->getOrCreateMaterial<CameraShader>(errorState);
		if (!errorState.check(video_material != nullptr, "%s: unable to get or create video material", resource->mID.c_str()))
			return false;

		// Create resource for the video material instance
		mMaterialInstanceResource.mBlendMode = EBlendMode::Opaque;
		mMaterialInstanceResource.mDepthMode = EDepthMode::NoReadWrite;
		mMaterialInstanceResource.mMaterial = video_material;

		// Initialize video material instance, used for rendering video
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
		mYSampler = ensureSampler(uniform::video::YSampler, errorState);
		mUSampler = ensureSampler(uniform::video::USampler, errorState);
		mVSampler = ensureSampler(uniform::video::VSampler, errorState);


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

		// Call on draw
		mTarget.beginRendering();
		onDraw(mTarget, command_buffer, glm::mat4(), proj_matrix);
		mTarget.endRendering();
	}




	void RenderOakCameraComponentInstance::setContentSampler()
	{


		if (mOakFrame != nullptr) {

			mYSampler->setTexture(mOakFrame->getYTexture());
			mUSampler->setTexture(mOakFrame->getUTexture());
			mVSampler->setTexture(mOakFrame->getVTexture());

		}
	}


	
}