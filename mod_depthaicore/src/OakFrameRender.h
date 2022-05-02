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


		void initTextures(glm::vec2 imgFrame, glm::vec2 sizeFrameNN = { -1, -1 });
		bool texturesInitDone();
		bool firstUpdateTensorData();

		glm::vec2 getOakFrameSize();

		void toPlanar(cv::Mat& bgr, std::vector<std::uint8_t>& data);
		void updateSamticSeg(cv::Mat* frame, std::shared_ptr<dai::RawBuffer> tensor, std::shared_ptr<dai::NNData> inDet);

		DepthAICoreService& getService(); 


	private :

		DepthAICoreService& mService;


		void updateOakFrame();
		bool init();

		bool initTexture(glm::vec2 imgFrame, glm::vec2 sizeFrameNN);

		std::unique_ptr<Texture2D> texRGBA;
		std::unique_ptr<Texture2D> texSegmentation;
		SurfaceDescriptor rgbaSurfaceDescriptor, segmentationSurfaceDescriptor;

		void clearTexture();

		bool texturesCreated;
		bool tensorData;
		glm::vec2 frameSize;


		cv::Mat* rgbaMat;

		// helper
		void checkCvMatType(cv::Mat texColor);

	};

	using OakFrameRenderObjectCreator = rtti::ObjectCreator<OakFrameRender, DepthAICoreService>;
	
}

