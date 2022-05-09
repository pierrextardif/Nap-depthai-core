#pragma once

// External Includes
#include <nap/device.h>
#include <nap/resourceptr.h>
#include <nap/numeric.h>
#include <texture2d.h>

#include <depthaicoreservice.h>

#include "depthai/depthai.hpp"

using namespace nap;

namespace nap
{
	class DepthAICoreService;


	
	class NAPAPI OakFrameRender : public Device
	{
		RTTI_ENABLE(Device)    
		friend class DepthAICoreService;

		
	public :
		
		OakFrameRender(DepthAICoreService& service);
		virtual bool start(utility::ErrorState& errorState) override;
		virtual void stop() override;

		Texture2D& getRGBATexture();
		Texture2D& getSegmentationTexture();


		void initTextures(glm::vec2 imgFrame, glm::vec2 offsetCrop, glm::vec2 sizeFrameNN, bool _humanIncropping);
		bool texturesInitDone();
		bool firstUpdateTensorData();

		glm::vec2 getOakFrameSize();
		glm::vec2 getCropOffset();
		float getOutsideHumanCropping();

		void updateSSMainTex(cv::Mat* colorFrame);
		void updateSSMaskTex(std::shared_ptr<dai::NNData> inDet);
		void updatePreview(cv::Mat* previewFrame);

		DepthAICoreService& getService(); 


	private :

		DepthAICoreService& mService;


		void updateOakFrame();
		bool init();

		bool initTexture(glm::vec2 imgFrame, glm::vec2 offsetCropNN, glm::vec2 sizeFrameNN, bool _humanIncropping);

		std::unique_ptr<Texture2D> texRGBA;
		std::unique_ptr<Texture2D> texSegmentation;
		SurfaceDescriptor rgbaSurfaceDescriptor, segmentationSurfaceDescriptor;

		void clearTexture();

		bool texturesCreated;
		glm::vec2 frameSize;
		glm::vec2 offsetCrop;
		bool croppingOutsideHumanFrame;


		cv::Mat* rgbaMat;

		// helper
		void checkCvMatType(cv::Mat texColor);

	};

	using OakFrameRenderObjectCreator = rtti::ObjectCreator<OakFrameRender, DepthAICoreService>;
	
}

