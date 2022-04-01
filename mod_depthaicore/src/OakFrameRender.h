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
	class DepthAiCoreService;


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
		friend class DepthAiCoreService;

		
	public :
		
		OakFrameRender(DepthAiCoreService& service);
		virtual bool start(utility::ErrorState& errorState) override;
		virtual void stop() override;


		void update(double deltaTime);

		//DAIIputOutput type = DAIIputOutput::InputLink;
		DAINodeType nodeType;

		Texture2D& getYTexture();
		Texture2D& getUTexture();
		Texture2D& getVTexture();


	private :

		DepthAiCoreService& mService;
		dai::Pipeline pipeline;

		dai::DeviceBase* internalDevice;

		void updateOakFrame();
		bool init();

		std::unique_ptr<Texture2D> texY;
		std::unique_ptr<Texture2D> texU;
		std::unique_ptr<Texture2D> texV;

		std::shared_ptr<dai::DataOutputQueue> video = nullptr;

		bool texturesCreated;

	};

	using OakFrameRenderObjectCreator = rtti::ObjectCreator<OakFrameRender, DepthAiCoreService>;
	
}

