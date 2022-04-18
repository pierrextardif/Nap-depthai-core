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




	
	class NAPAPI TestFrameRender : public Device
	{
		RTTI_ENABLE(Device)    
		friend class DepthAICoreService;

		
	public :
		
		TestFrameRender(DepthAICoreService& service);
		virtual bool start(utility::ErrorState& errorState) override;
		virtual void stop() override;


		void update(double deltaTime);


		Texture2D& getRGBATexture();

		bool textureInit();

		glm::vec2 getOakFrameSize();


		DepthAICoreService& getService(); 


	private :

		DepthAICoreService& mService;
		dai::Pipeline pipeline;

		dai::Device* device;

		void updateOakFrame();
		bool init();

		bool initTexture(std::shared_ptr < dai::ImgFrame > imgFrame);

		std::unique_ptr<Texture2D> texRGBA;
		SurfaceDescriptor rgbaSurfaceDescriptor;

		std::shared_ptr<dai::DataOutputQueue> video = nullptr;

		void clearTexture();

		bool texturesCreated;
		glm::vec2 frameSize;
		int pitch;

		uint8_t* tmpBuffer;

		// helper
		std::vector<uint8_t> createUVTexture(bool vTex);
		void checkCvMatType(cv::Mat texColor);

	};

	using TestFrameRenderObjectCreator = rtti::ObjectCreator<TestFrameRender, DepthAICoreService>;
	
}

