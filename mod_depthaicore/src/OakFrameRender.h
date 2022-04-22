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


	enum class DAINodeType : int
	{
		MonoCamera = 1,
		ColorCamera = 2,
		EdgeDetector = 3,
		ImageManip = 10,
		FeatureTracker = 11,
		ObjectTracker = 12,
		XLinkIn = 20,
		XLinkOut = 21,
		NeuralNetwork = 30,
		MobileNetDetectionNetwork = 31,
		StereoDepth = 40,
		VideoEncoder = 41,
		IMU = 50,

	};


	
	class NAPAPI OakFrameRender : public Device
	{
		RTTI_ENABLE(Device)    
		friend class DepthAICoreService;

		
	public :
		
		OakFrameRender(DepthAICoreService& service);
		virtual bool start(utility::ErrorState& errorState) override;
		virtual void stop() override;


		void update(double deltaTime);

		//DAIIputOutput type = DAIIputOutput::InputLink;
		DAINodeType nodeType;
		std::string nnPath = "C:/Users/pierr/Documents/DEV/NAP/NAP-0.5.0-Win64-x86_64/user_modules/mod_depthaicore/data/nn/deeplabv3p_person_6_shaves.blob";

		Texture2D& getRGBATexture();
		Texture2D& getSegmentationTexture();

		bool textureInit();

		glm::vec2 getOakFrameSize();


		DepthAICoreService& getService(); 


	private :

		DepthAICoreService& mService;
		dai::Pipeline pipeline;

		dai::Device* device;

		void updateOakFrame();
		bool init();

		bool initTexture(std::shared_ptr < dai::ImgFrame > imgFrame, glm::vec2 sizeFrameNN);

		std::unique_ptr<Texture2D> texRGBA;
		std::unique_ptr<Texture2D> texSegmentation;
		SurfaceDescriptor rgbaSurfaceDescriptor, segmentationSurfaceDescriptor;

		std::shared_ptr<dai::DataOutputQueue> video = nullptr;
		std::shared_ptr<dai::DataOutputQueue> qRgb = nullptr;
		std::shared_ptr<dai::DataOutputQueue> qNN = nullptr;
		std::shared_ptr<dai::DataInputQueue> inDataInQueue = nullptr;

		std::shared_ptr<dai::RawBuffer> tensor = nullptr;

		void clearTexture();

		bool texturesCreated;
		glm::vec2 frameSize;
		glm::vec2 previewSize;
		int pitch;

		



		cv::Mat* rgbaMat;

		// helper
		std::vector<uint8_t> createUVTexture(bool vTex);
		void checkCvMatType(cv::Mat texColor);

	};

	using OakFrameRenderObjectCreator = rtti::ObjectCreator<OakFrameRender, DepthAICoreService>;
	
}

