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




	class NAPAPI ColorCameraNode : public Device
	{
		RTTI_ENABLE(Device)
			friend class DepthAICoreService;


	public:

		ColorCameraNode(DepthAICoreService& service);
		virtual bool start(utility::ErrorState& errorState) override;
		virtual void stop() override;


		void update(double deltaTime);

		int previewSize = 256;
		dai::CameraBoardSocket cameraBoardSocket = dai::CameraBoardSocket::RGB;
		dai::ColorCameraProperties::SensorResolution sensorResolution = dai::ColorCameraProperties::SensorResolution::THE_1080_P;
		bool interleaved = false;
		dai::ColorCameraProperties::ColorOrder colorOrder = dai::ColorCameraProperties::ColorOrder::RGB;

	private:

		dai::Pipeline pipeline;
		std::shared_ptr < dai::node::ColorCamera > cam;


	};

	using ColorCameraNodeObjectCreator = rtti::ObjectCreator<ColorCameraNode, DepthAICoreService>;

}




