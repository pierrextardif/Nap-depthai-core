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
    OakFrameRender::OakFrameRender(DepthAICoreService& service) :
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
        camRgb->setInterleaved(false);
        camRgb->setColorOrder(dai::ColorCameraProperties::ColorOrder::RGB);

        xoutVideo->input.setBlocking(false);
        xoutVideo->input.setQueueSize(1);

        // Linking
        camRgb->video.link(xoutVideo->input);

        // Connect to device and start pipeline
        device = new dai::Device(pipeline);

        video = device->getOutputQueue("video");

        return true;
    }


    void OakFrameRender::update(double deltatime) {
        updateOakFrame();
    }


    void OakFrameRender::updateOakFrame() {
        if(video){
            std::shared_ptr < dai::ImgFrame > videoIn = video->tryGet<dai::ImgFrame>();

            if (videoIn && texturesCreated) {

                assert(texRGBA != nullptr);
                //texRGBA->update(videoIn->getFrame()

                cv::Mat tmpColor = videoIn->getFrame(true);
                //cv::imshow("rgb", tmpColor);
                /*cv::Mat* dataMat = new cv::Mat();
                cv::cvtColor(videoIn->getFrame(), *dataMat, cv::COLOR_RGB2RGBA, 4);*/

                assert(tmpColor.data != nullptr);

                size_t sizeToCopy = frameSize.x * frameSize.y;

                

                memcpy(tmpBuffer, tmpColor.data, sizeToCopy);

                texRGBA->update(static_cast<const void *>(tmpBuffer), rgbaSurfaceDescriptor);

            }
            else {
                texturesCreated = initTexture(videoIn);
            }

        }
    }
    bool OakFrameRender::initTexture(std::shared_ptr < dai::ImgFrame > imgFrame)
    {
        if (texturesCreated) {
            return texturesCreated;
        }
        else if(imgFrame){
            utility::ErrorState error;
            if (imgFrame->getWidth()!= 0 && imgFrame->getHeight()!= 0) {

                frameSize = { imgFrame->getWidth(), imgFrame->getHeight() };
                pitch = frameSize.x * 3;



                
                rgbaSurfaceDescriptor.mWidth = frameSize.x;
                rgbaSurfaceDescriptor.mHeight = frameSize.y;
                rgbaSurfaceDescriptor.mColorSpace = EColorSpace::Linear;
                rgbaSurfaceDescriptor.mDataType = ESurfaceDataType::BYTE;
                rgbaSurfaceDescriptor.mChannels = ESurfaceChannels::RGBA;
                texRGBA = std::make_unique<Texture2D>(mService.getCore());
                texRGBA->mUsage = ETextureUsage::DynamicWrite;
                if (!texRGBA->init(rgbaSurfaceDescriptor, false, 0, error))
                    return false;

                const size_t sizeFrame = static_cast<const size_t>(rgbaSurfaceDescriptor.getSizeInBytes());
                tmpBuffer = new uint8_t[sizeFrame];


                clearTexture();

                texturesCreated = true;
                std::cout << "texture created" << std::endl;

            }
        }
        else {
            return false;
        }
    }


    void OakFrameRender::clearTexture()
    {
        float vid_x = frameSize.x;
        float vid_y = frameSize.y;
        float uv_x = vid_x * 0.5f;
        float uv_y = vid_y * 0.5f;

        // YUV420p to RGB conversion uses an 'offset' value of (-0.0625, -0.5, -0.5) in the shader. 
        // This means that initializing the YUV planes to zero does not actually result in black output.
        // To fix this, we initialize the YUV planes to the negative of the offset
        std::vector<uint8_t> y_default_data(vid_x * vid_y, 16);
        std::vector<uint8_t> rgba_default_data(vid_x * vid_y * 4, 16);

        // Initialize UV planes
        std::vector<uint8_t> uv_default_data(uv_x * uv_y, 127);



        texRGBA->update(rgba_default_data.data(), texRGBA->getWidth(), texRGBA->getHeight(), texRGBA->getWidth() * 4, ESurfaceChannels::RGBA);
    }

    nap::Texture2D& OakFrameRender::getRGBATexture() {

        NAP_ASSERT_MSG(texRGBA != nullptr, "Missing video RGBA texture");
        return *texRGBA;
    }


    std::vector<uint8_t> OakFrameRender::createUVTexture(bool vTex) {
        std::vector < uint8> uv_data;


        for (int i = 0; i < frameSize.x; i++) {
            for (int j = 0; j < frameSize.y; j++) {
                
                uint8 uvCoord = i;
                if (vTex)uvCoord = j;
                uv_data.push_back(uvCoord);
            }
        }

        return uv_data;


    }

    bool OakFrameRender::textureInit() {

        return texturesCreated;
    }



    void OakFrameRender::checkCvMatType(cv::Mat texColor) {
        int inttype = texColor.type();

        std::string r, a;
        uchar depth = inttype & CV_MAT_DEPTH_MASK;
        uchar chans = 1 + (inttype >> CV_CN_SHIFT);
        switch (depth) {
        case CV_8U:  r = "8U";   a = "Mat.at<uchar>(y,x)"; break;
        case CV_8S:  r = "8S";   a = "Mat.at<schar>(y,x)"; break;
        case CV_16U: r = "16U";  a = "Mat.at<ushort>(y,x)"; break;
        case CV_16S: r = "16S";  a = "Mat.at<short>(y,x)"; break;
        case CV_32S: r = "32S";  a = "Mat.at<int>(y,x)"; break;
        case CV_32F: r = "32F";  a = "Mat.at<float>(y,x)"; break;
        case CV_64F: r = "64F";  a = "Mat.at<double>(y,x)"; break;
        default:     r = "User"; a = "Mat.at<UKNOWN>(y,x)"; break;
        }
        r += "C";
        r += (chans + '0');
        std::cout << "Mat is of type " << r << " and should be accessed with " << a << std::endl;
    }


    glm::vec2 OakFrameRender::getOakFrameSize() {

        return frameSize;
    }
}
