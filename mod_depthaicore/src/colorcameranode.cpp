#include "colorcameranode.h"

#include <depthaicoreservice.h>
//external includes
#include <mathutils.h>
#include <nap/assert.h>
#include <rtti/typeinfo.h>


RTTI_BEGIN_ENUM(dai::CameraBoardSocket)
RTTI_ENUM_VALUE(dai::CameraBoardSocket::AUTO, "AUTO"),
RTTI_ENUM_VALUE(dai::CameraBoardSocket::RGB, "RGB"),
RTTI_ENUM_VALUE(dai::CameraBoardSocket::LEFT, "LEFT"),
RTTI_ENUM_VALUE(dai::CameraBoardSocket::RIGHT, "RIGHT")
RTTI_END_ENUM


RTTI_BEGIN_ENUM(dai::ColorCameraProperties::SensorResolution)
RTTI_ENUM_VALUE(dai::ColorCameraProperties::SensorResolution::THE_1080_P, "1080_P"),
RTTI_ENUM_VALUE(dai::ColorCameraProperties::SensorResolution::THE_4_K, "4K"),
RTTI_ENUM_VALUE(dai::ColorCameraProperties::SensorResolution::THE_12_MP, "12MP"),
RTTI_ENUM_VALUE(dai::ColorCameraProperties::SensorResolution::THE_13_MP, "13MP")
RTTI_END_ENUM

RTTI_BEGIN_ENUM(dai::ColorCameraProperties::ColorOrder)
RTTI_ENUM_VALUE(dai::ColorCameraProperties::ColorOrder::BGR, "BGR"),
RTTI_ENUM_VALUE(dai::ColorCameraProperties::ColorOrder::RGB, "RGB")
RTTI_END_ENUM


// nap::rendervideototexturecomponentInstance run time class definition 
RTTI_BEGIN_CLASS_NO_DEFAULT_CONSTRUCTOR(nap::ColorCameraNode)
RTTI_PROPERTY("preview Size", &nap::ColorCameraNode::previewSize, nap::rtti::EPropertyMetaData::Default)
RTTI_PROPERTY("cam board socket", &nap::ColorCameraNode::cameraBoardSocket, nap::rtti::EPropertyMetaData::Default)
RTTI_PROPERTY("sensor resolution", &nap::ColorCameraNode::sensorResolution, nap::rtti::EPropertyMetaData::Default)
RTTI_PROPERTY("interleaved", &nap::ColorCameraNode::interleaved, nap::rtti::EPropertyMetaData::Default);
RTTI_PROPERTY("Color order", &nap::ColorCameraNode::colorOrder, nap::rtti::EPropertyMetaData::Default);

RTTI_END_CLASS

//////////////////////////////////////////////////////////////////////////


namespace nap
{
	ColorCameraNode::ColorCameraNode(DepthAICoreService& service):
		mService(service)
	{
		pipeline = mService.getPipeline();

	}

	std::shared_ptr < dai::node::ColorCamera > ColorCameraNode::getCam() 
	{
		return cam;
	}

	void ColorCameraNode::initCC()
	{


		cam = pipeline->create<dai::node::ColorCamera>();
		
		// Properties
		cam->setPreviewSize(previewSize, previewSize);
		cam->setBoardSocket(cameraBoardSocket);
		cam->setResolution(sensorResolution);
		cam->setInterleaved(interleaved);
		cam->setColorOrder(colorOrder);
	}



	bool ColorCameraNode::start(utility::ErrorState& errorState) {
		
		initCC();
		return true;
	}

	void ColorCameraNode::stop()
	{

	}


	void ColorCameraNode::update(double deltaTime)
	{
	}


}