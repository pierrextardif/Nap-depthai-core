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

    DepthAICoreService& OakFrameRender::getService(){
        return mService;
    }


    bool OakFrameRender::init() {

        //std::cout << "init!!" << std::endl;

        //previewSize = { 256, 256 };

        //pipeline.setOpenVINOVersion(dai::OpenVINO::VERSION_2021_4);

        //// Define source and output
        //auto camRgb = pipeline.create<dai::node::ColorCamera>();
        //auto xin = pipeline.create<dai::node::XLinkIn>();
        //auto xoutRGB = pipeline.create<dai::node::XLinkOut>();
        //auto nnOut = pipeline.create<dai::node::XLinkOut>();

        //// neural network node
        //auto detectionNN = pipeline.create<dai::node::NeuralNetwork>();
        ////detectionNN->setBlobPath(nnPath);
        //detectionNN->setNumInferenceThreads(2);
        //detectionNN->input.setBlocking(false);

        //xoutRGB->setStreamName("rgb");
        //xoutRGB->getStreamName();
        //nnOut->setStreamName("segmentation");

        //xin->setStreamName("nn_in");
        //nnOut->setStreamName("segmentation");

        //// Properties
        //camRgb->setPreviewSize(previewSize.x, previewSize.x);
        //camRgb->setBoardSocket(dai::CameraBoardSocket::RGB);
        //camRgb->setResolution(dai::ColorCameraProperties::SensorResolution::THE_1080_P);
        //camRgb->setInterleaved(false);
        //camRgb->setColorOrder(dai::ColorCameraProperties::ColorOrder::RGB);


        //xin->setMaxDataSize(300 * 300 * 3);
        //xin->setNumFrames(30);

        //camRgb->preview.link(detectionNN->input);
        //detectionNN->passthrough.link(xoutRGB->input);

        //// Linking
        ////camRgb->video.link(xoutRGB->input);

        //xin->out.link(detectionNN->input);


        //detectionNN->out.link(nnOut->input);

        //// Connect to device and start pipeline
        //device = new dai::Device(pipeline);


        //qRgb = device->getOutputQueue("rgb", 4, false);
        //qNN = device->getOutputQueue("segmentation", 4, false);
        //inDataInQueue = device->getInputQueue("nn_in");

        //tensor = std::make_shared<dai::RawBuffer>();

        return true;

    }


    void OakFrameRender::update(double deltatime) {
        updateOakFrame();
    }

    // from utilities of depthai-core/examples/utilities
    void toPlanar(cv::Mat& bgr, std::vector<std::uint8_t>& data) {

        data.resize(bgr.cols * bgr.rows * 3);
        for (int y = 0; y < bgr.rows; y++) {
            for (int x = 0; x < bgr.cols; x++) {
                auto p = bgr.at<cv::Vec3b>(y, x);
                data[x + y * bgr.cols + 0 * bgr.rows * bgr.cols] = p[0];
                data[x + y * bgr.cols + 1 * bgr.rows * bgr.cols] = p[1];
                data[x + y * bgr.cols + 2 * bgr.rows * bgr.cols] = p[2];
            }
        }
    }

    void OakFrameRender::updateOakFrame() {
        //if(qRgb){


        //    std::shared_ptr<dai::ImgFrame> inRgb = qRgb->tryGet<dai::ImgFrame>();
        //    std::shared_ptr<dai::NNData> inDet = qNN->tryGet<dai::NNData>();


        //    if (inRgb && inDet && texRGBA != nullptr){
        //        cv::Mat frame = inRgb->getCvFrame();
        //        toPlanar(frame, tensor->data);

        //        cv::cvtColor(frame, *rgbaMat, cv::COLOR_RGB2RGBA);
        //        texRGBA->update((uint8_t*)rgbaMat->data, rgbaSurfaceDescriptor);
        //        
        //        inDataInQueue->send(tensor);



        //        std::vector<dai::TensorInfo> vecAllLayers = inDet->getAllLayers();
        //        if (vecAllLayers.size() > 0) {

        //            std::vector<std::int32_t> layer1 = inDet->getLayerInt32(vecAllLayers[0].name);
        //            std::vector<float> layer2 = inDet->getLayerFp16(vecAllLayers[0].name);
        //            std::vector < uint8_t> layerU = inDet->getFirstLayerUInt8();
        //            if(layer1.size() > 0)
        //                texSegmentation->update((uint8_t*)layer1.data(), segmentationSurfaceDescriptor);
        //        }

        //    }
        //    else {
        //        texturesCreated = initTexture(inRgb, previewSize);
        //    }
        //}
    }
    bool OakFrameRender::initTexture(std::shared_ptr < dai::ImgFrame > imgFrame, glm::vec2 sizeFrameNN)
    {
        if (texturesCreated) {
            return texturesCreated;
        }
        else if(imgFrame){
            utility::ErrorState error;
            if (imgFrame->getWidth()!= 0 && imgFrame->getHeight()!= 0) {

                frameSize = { imgFrame->getWidth(), imgFrame->getHeight() };
                
                rgbaSurfaceDescriptor.mWidth = frameSize.x;
                rgbaSurfaceDescriptor.mHeight = frameSize.y;
                rgbaSurfaceDescriptor.mColorSpace = EColorSpace::Linear;
                rgbaSurfaceDescriptor.mDataType = ESurfaceDataType::BYTE;
                rgbaSurfaceDescriptor.mChannels = ESurfaceChannels::BGRA;
                texRGBA = std::make_unique<Texture2D>(mService.getCore());
                texRGBA->mUsage = ETextureUsage::DynamicWrite;
                if (!texRGBA->init(rgbaSurfaceDescriptor, false, 0, error))
                    return false;

                segmentationSurfaceDescriptor.mWidth = sizeFrameNN.x;
                segmentationSurfaceDescriptor.mHeight = sizeFrameNN.y;
                segmentationSurfaceDescriptor.mColorSpace = EColorSpace::Linear;
                segmentationSurfaceDescriptor.mDataType = ESurfaceDataType::BYTE;
                segmentationSurfaceDescriptor.mChannels = ESurfaceChannels::RGBA;
                texSegmentation = std::make_unique<Texture2D>(mService.getCore());
                texSegmentation->mUsage = ETextureUsage::DynamicWrite;
                if (!texSegmentation->init(segmentationSurfaceDescriptor, false, 0, error))
                    return false;


                const size_t sizeFrame = static_cast<const size_t>(rgbaSurfaceDescriptor.getSizeInBytes());
                rgbaMat = new cv::Mat(frameSize.x, frameSize.y, CV_8UC4);
                *rgbaMat = cv::Scalar::all(255);


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


        std::vector<uint8_t> y_default_data(vid_x * vid_y, 16);
        std::vector<uint8_t> rgba_default_data(vid_x * vid_y * 4, 16);



        texRGBA->update(rgba_default_data.data(), texRGBA->getWidth(), texRGBA->getHeight(), texRGBA->getWidth() * 4, ESurfaceChannels::RGBA);
    }

    nap::Texture2D& OakFrameRender::getRGBATexture() {

        NAP_ASSERT_MSG(texRGBA != nullptr, "Missing video RGBA texture");
        return *texRGBA;
    }

    nap::Texture2D& OakFrameRender::getSegmentationTexture() {

        NAP_ASSERT_MSG(texSegmentation != nullptr, "Missing video RGBA texture");
        return *texSegmentation;
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
