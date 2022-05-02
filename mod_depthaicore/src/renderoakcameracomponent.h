#pragma once

// External Includes
#include <rendercomponent.h>
#include <nap/resourceptr.h>
#include <rendertexture2d.h>
#include <planemesh.h>
#include <rendertarget.h>
#include <color.h>
#include <materialinstance.h>
#include <renderablemesh.h>

#include <oakframerender.h>

namespace nap
{

	class RenderOakCameraComponentInstance;

	class NAPAPI RenderOakCameraComponent : public RenderableComponent
	{
		RTTI_ENABLE(RenderableComponent)
		DECLARE_COMPONENT(RenderOakCameraComponent, RenderOakCameraComponentInstance)
	public:

		ResourcePtr<RenderTexture2D>	mOutputTexture = nullptr;
		ResourcePtr<OakFrameRender>		mOakFrame = nullptr;
		ERasterizationSamples			mRequestedSamples = ERasterizationSamples::One;		///< Property: 'Samples' The number of samples used during Rasterization. For better results enable 'SampleShading'
		RGBAColor8						mClearColor = { 255, 255, 255, 255 };

	};


	class NAPAPI RenderOakCameraComponentInstance : public RenderableComponentInstance
	{
		RTTI_ENABLE(RenderableComponentInstance)
	public:
		RenderOakCameraComponentInstance(EntityInstance& entity, Component& resource);

		virtual bool init(utility::ErrorState& errorState) override;
		virtual bool isSupported(nap::CameraComponentInstance& camera) const override;
		
		OakFrameRender* getOakFrameRender() {
			return mOakFrame;
		}

		bool setContentSampler();
		void draw();

		nap::Texture2D& getOutputTexture();
		glm::vec2 getOakFrameSize();

	private :
		OakFrameRender*				mOakFrame = nullptr;
		RenderTexture2D*			mOutputTexture = nullptr;						///< Texture currently bound by target
		RGBColorFloat				mClearColor = { 0.0f, 0.0f, 0.0f };				///< Target Clear Color
		RenderTarget				mTarget;										///< Target video is rendered into
		PlaneMesh					mPlane;											///< Plane that is rendered
		MaterialInstance			mMaterialInstance;								///< The MaterialInstance as created from the resource.
		MaterialInstanceResource	mMaterialInstanceResource;						///< Resource used to initialize the material instance
		RenderableMesh				mRenderableMesh;								///< Valid Plane / Material combination
		RenderService*				mRenderService = nullptr;						///< Pointer to the render service
		UniformMat4Instance*		mModelMatrixUniform = nullptr;					///< Model matrix uniform in the material
		UniformMat4Instance*		mProjectMatrixUniform = nullptr;				///< Projection matrix uniform in the material
		UniformMat4Instance*		mViewMatrixUniform = nullptr;					///< View matrix uniform in the material
		UniformStructInstance*		mMVPStruct = nullptr;							///< model view projection struct
		Sampler2DInstance*			mRGBASampler = nullptr;							///< Video material Y sampler
		Sampler2DInstance*			mSemanticSegSampler = nullptr;							///< Video material Y sampler
		glm::mat4x4					mModelMatrix;									///< Computed model matrix, used to scale plane to fit target bounds
		bool						mDirty = true;	


		bool hasBeenSet = false;
		/**
		 * Checks if the uniform is available on the source material and creates it if so
		 * @return the uniform, nullptr if not available.
		 */
		UniformMat4Instance* ensureUniform(const std::string& uniformName, utility::ErrorState& error);

		/**
		 * Checks if the sampler with the given name is available on the source material and creates it if so
		 * @return new or created sampler
		 */
		Sampler2DInstance* ensureSampler(const std::string& samplerName, utility::ErrorState& error);

	protected :

		virtual void onDraw(IRenderTarget& renderTarget, VkCommandBuffer commandBuffer, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) override;

	};

}


