#include "testframerender.h"

#include <depthaicoreservice.h>

 //external includes
#include <mathutils.h>
#include <nap/assert.h>
#include <rtti/typeinfo.h>



// nap::rendervideototexturecomponentInstance run time class definition 
RTTI_BEGIN_CLASS_NO_DEFAULT_CONSTRUCTOR(nap::TestFrameRender)

RTTI_END_CLASS

//////////////////////////////////////////////////////////////////////////



namespace nap
{
    TestFrameRender::TestFrameRender(DepthAICoreService& service) :
        mService(service) {

        texturesCreated = false;
        //std::cout << "creation of the TestFrameRenderer done" << std::endl;
    }

    bool TestFrameRender::start(utility::ErrorState& errorState) {

        mService.registerTestFrame(*this);


        return true;
    }

    void TestFrameRender::stop() {

    }

    DepthAICoreService& TestFrameRender::getService(){
        return mService;
    }


    bool TestFrameRender::init() {



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


    void TestFrameRender::update(double deltatime) {
        updateOakFrame();
    }


    void TestFrameRender::updateOakFrame() {
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
    bool TestFrameRender::initTexture(std::shared_ptr < dai::ImgFrame > imgFrame)
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


    void TestFrameRender::clearTexture()
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

    nap::Texture2D& TestFrameRender::getRGBATexture() {

        NAP_ASSERT_MSG(texRGBA != nullptr, "Missing video RGBA texture");
        return *texRGBA;
    }


    std::vector<uint8_t> TestFrameRender::createUVTexture(bool vTex) {
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

    bool TestFrameRender::textureInit() {

        return texturesCreated;
    }



    void TestFrameRender::checkCvMatType(cv::Mat texColor) {
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


    glm::vec2 TestFrameRender::getOakFrameSize() {

        return frameSize;
    }
}
