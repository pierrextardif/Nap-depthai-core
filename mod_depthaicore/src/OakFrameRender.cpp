#include "oakframerender.h"

#include <depthaicoreservice.h>

 //external includes
#include <mathutils.h>
#include <nap/assert.h>
#include <rtti/typeinfo.h>


// nap::rendervideototexturecomponentInstance run time class definition 
RTTI_BEGIN_CLASS_NO_DEFAULT_CONSTRUCTOR(nap::OakFrameRender)
RTTI_END_CLASS

//////////////////////////////////////////////////////////////////////////



namespace nap
{
    OakFrameRender::OakFrameRender(DepthAICoreService& service) :
        mService(service) {

        texturesCreated = false;
    }

    bool OakFrameRender::start(utility::ErrorState& errorState) {

        return true;
    }

    void OakFrameRender::stop() {

    }

    DepthAICoreService& OakFrameRender::getService(){
        return mService;
    }


    bool OakFrameRender::init() {
        return true;
    }

    void OakFrameRender::updateSSMainTex(cv::Mat* colorFrame)
    {
        cv::cvtColor(*colorFrame, *rgbaMat, cv::COLOR_RGB2RGBA);
        texRGBA->update((uint8_t*)rgbaMat->data, rgbaSurfaceDescriptor);

    }

    void OakFrameRender::updateSSMaskTex(std::shared_ptr<dai::NNData> inDet)
    {
        std::vector<dai::TensorInfo> vecAllLayers = inDet->getAllLayers();

        if (vecAllLayers.size() > 0) {

            std::vector<std::int32_t> layer1 = inDet->getLayerInt32(vecAllLayers[0].name);
            if (layer1.size() > 0) {
                texSegmentation->update((uint8_t*)layer1.data(), segmentationSurfaceDescriptor);
            }
        }
    }



    void OakFrameRender::updatePreview(cv::Mat* previewFrame)
    {
        cv::Mat* colMatTest = new cv::Mat(256, 256, CV_8UC4);
        cv::cvtColor(*previewFrame, *colMatTest, cv::COLOR_RGB2RGBA);
        texSegmentation->update((uint8_t*)colMatTest->data, segmentationSurfaceDescriptor);
    }

    bool OakFrameRender::texturesInitDone() {

        return texturesCreated;
    }


    void OakFrameRender::initTextures(glm::vec2 imgFrame, glm::vec2 offsetCrop, glm::vec2 sizeFrameNN, bool _humanIncropping)
    {
        texturesCreated = initTexture(imgFrame, offsetCrop, sizeFrameNN, _humanIncropping);
    }

    bool OakFrameRender::initTexture(glm::vec2 imgFrame, glm::vec2 offsetCropNN, glm::vec2 sizeFrameNN, bool _humanIncropping)
    {
        if (texturesCreated) {
            return texturesCreated;
        }else{
            utility::ErrorState error;

            frameSize = imgFrame;
                
            rgbaSurfaceDescriptor.mWidth = frameSize.x;
            rgbaSurfaceDescriptor.mHeight = frameSize.y;
            rgbaSurfaceDescriptor.mColorSpace = EColorSpace::Linear;
            rgbaSurfaceDescriptor.mDataType = ESurfaceDataType::BYTE;
            rgbaSurfaceDescriptor.mChannels = ESurfaceChannels::BGRA;
            texRGBA = std::make_unique<Texture2D>(mService.getCore());
            texRGBA->mUsage = ETextureUsage::DynamicWrite;
            if (!texRGBA->init(rgbaSurfaceDescriptor, false, 0, error))
                return false;

            if (sizeFrameNN.x != -1 && sizeFrameNN.y != -1) {
                segmentationSurfaceDescriptor.mWidth = sizeFrameNN.x;
                segmentationSurfaceDescriptor.mHeight = sizeFrameNN.y;
                segmentationSurfaceDescriptor.mColorSpace = EColorSpace::Linear;
                segmentationSurfaceDescriptor.mDataType = ESurfaceDataType::BYTE;
                segmentationSurfaceDescriptor.mChannels = ESurfaceChannels::RGBA;
                texSegmentation = std::make_unique<Texture2D>(mService.getCore());
                texSegmentation->mUsage = ETextureUsage::DynamicWrite;
                if (!texSegmentation->init(segmentationSurfaceDescriptor, false, 0, error))
                    return false;
            }


            const size_t sizeFrame = static_cast<const size_t>(rgbaSurfaceDescriptor.getSizeInBytes());
            rgbaMat = new cv::Mat(frameSize.x, frameSize.y, CV_8UC4);
            *rgbaMat = cv::Scalar::all(255);


            clearTexture();

            offsetCrop = offsetCropNN;
            croppingOutsideHumanFrame = _humanIncropping;

            return true;

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

        NAP_ASSERT_MSG(texSegmentation != nullptr, "Missing video segmentation texture");
        return *texSegmentation;
    }

    glm::vec2 OakFrameRender::getOakFrameSize() {

        return frameSize;
    }

    glm::vec2 OakFrameRender::getCropOffset() {
        return offsetCrop;
    }

    float OakFrameRender::getOutsideHumanCropping()
    {
        return croppingOutsideHumanFrame ? 1.0 : 0.;
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
}
