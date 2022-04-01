#include "oakframerender.h"

#include <depthaicoreservice.h>

 //external includes
#include <mathutils.h>
#include <nap/assert.h>
#include <rtti/typeinfo.h>


RTTI_BEGIN_ENUM(nap::DAINodeType)
RTTI_ENUM_VALUE(nap::DAINodeType::MonoCamera, "MonoCamera"),
RTTI_ENUM_VALUE(nap::DAINodeType::ColorCamera, "ColorCamera"),
RTTI_ENUM_VALUE(nap::DAINodeType::EdgeDetector, "EdgeDetector"),
RTTI_ENUM_VALUE(nap::DAINodeType::ImageManip, "ImageManip"),
RTTI_ENUM_VALUE(nap::DAINodeType::FeatureTracker, "FeatureTracker"),
RTTI_ENUM_VALUE(nap::DAINodeType::ObjectTracker, "ObjectTracker"),
RTTI_ENUM_VALUE(nap::DAINodeType::XLinkIn, "XLinkIn"),
RTTI_ENUM_VALUE(nap::DAINodeType::XLinkOut, "XLinkOut"),
RTTI_ENUM_VALUE(nap::DAINodeType::NeuralNetwork, "NeuralNetwork"),
RTTI_ENUM_VALUE(nap::DAINodeType::MobileNetDetectionNetwork, "MobileNetDetectionNetwork"),
RTTI_ENUM_VALUE(nap::DAINodeType::StereoDepth, "StereoDepth"),
RTTI_ENUM_VALUE(nap::DAINodeType::VideoEncoder, "VideoEncoder"),
RTTI_ENUM_VALUE(nap::DAINodeType::IMU, "IMU")
RTTI_END_ENUM



// nap::rendervideototexturecomponentInstance run time class definition 
RTTI_BEGIN_CLASS_NO_DEFAULT_CONSTRUCTOR(nap::OakFrameRender)
    RTTI_PROPERTY("node Type", &nap::OakFrameRender::nodeType, nap::rtti::EPropertyMetaData::Required)

RTTI_END_CLASS

//////////////////////////////////////////////////////////////////////////



namespace nap
{
    OakFrameRender::OakFrameRender(DepthAiCoreService& service) :
        mService(service) {

        texturesCreated = false;
        //std::cout << "creation of the oakframerenderer done" << std::endl;
    }

    bool OakFrameRender::start(utility::ErrorState& errorState) {

        mService.registerOakFrame(*this);


        return true;
    }

    void OakFrameRender::stop() {

    }


    bool OakFrameRender::init() {



        std::cout << "init!!" << std::endl;

        // Define source and output
        auto camRgb = pipeline.create<dai::node::ColorCamera>();
        auto xoutVideo = pipeline.create<dai::node::XLinkOut>();

        xoutVideo->setStreamName("video");

        // Properties
        camRgb->setBoardSocket(dai::CameraBoardSocket::RGB);
        camRgb->setResolution(dai::ColorCameraProperties::SensorResolution::THE_1080_P);
        camRgb->setVideoSize(1920, 1080);

        xoutVideo->input.setBlocking(false);
        xoutVideo->input.setQueueSize(1);

        // Linking
        camRgb->video.link(xoutVideo->input);

        // Connect to device and start pipeline
        dai::Device device(pipeline);
        internalDevice = &device;

        //video = internalDevice->getOutputQueue("video");

        return true;
    }


    void OakFrameRender::update(double deltatime) {
        updateOakFrame();
    }


    void OakFrameRender::updateOakFrame() {
        if(video){
        //auto videoIn = video->get<dai::ImgFrame>();
        }
        else {
            texturesCreated = init();
        }
    }


    nap::Texture2D& OakFrameRender::getYTexture()
    {
        NAP_ASSERT_MSG(texY != nullptr, "Missing video Y texture");
        return *texY;
    }


    nap::Texture2D& OakFrameRender::getUTexture()
    {
        NAP_ASSERT_MSG(texU != nullptr, "Missing video U texture");
        return *texU;
    }


    nap::Texture2D& OakFrameRender::getVTexture()
    {
        NAP_ASSERT_MSG(texV != nullptr, "Missing video V texture");
        return *texV;
    }
}
