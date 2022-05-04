#include "semanticseg.h"


#include <entity.h>
#include <algorithm>


RTTI_BEGIN_CLASS(nap::SemanticSegComponent)
RTTI_PROPERTY("color Cam Node", &nap::SemanticSegComponent::camRgb, nap::rtti::EPropertyMetaData::Required);
RTTI_PROPERTY("Neural Network Node", &nap::SemanticSegComponent::detectionNN, nap::rtti::EPropertyMetaData::Required);
RTTI_PROPERTY("OAK frame", &nap::SemanticSegComponent::mOakFrame, nap::rtti::EPropertyMetaData::Required);
RTTI_END_CLASS



RTTI_BEGIN_CLASS_NO_DEFAULT_CONSTRUCTOR(nap::SemanticSegComponentInstance)
RTTI_CONSTRUCTOR(nap::EntityInstance&, nap::Component&)
RTTI_END_CLASS


//////////////////////////////////////////////////////////////////////////


namespace nap
{
    bool SemanticSegComponentInstance::init(utility::ErrorState & errorState)
    {
        nap::SemanticSegComponent* resource = getComponent<nap::SemanticSegComponent>();
        camRgbNode = resource->camRgb.get();
		detectionNNNode = resource->detectionNN.get();
        mOakFrame = resource->mOakFrame.get();

        previewSize = { 256, 256 };
        //camRgbNode->getPreviewSize();

        pipeline = detectionNNNode->getPipelinePointer();
        
        initDAI();


        return true;
    }



    glm::vec2 SemanticSegComponentInstance::getSizeNN()
    {
        return previewSize;
    }

    void SemanticSegComponentInstance::initDAI()
	{

        pipeline->setOpenVINOVersion(dai::OpenVINO::VERSION_2021_4);

        // Define source and output
        auto xin = pipeline->create<dai::node::XLinkIn>();
        auto xoutPreview = pipeline->create<dai::node::XLinkOut>();
        auto xoutRgb = pipeline->create<dai::node::XLinkOut>();
        auto nnOut = pipeline->create<dai::node::XLinkOut>();

        xoutPreview->setStreamName("previewNN");
        xoutRgb->setStreamName("rgb");
        xin->setStreamName("nn_in");
        nnOut->setStreamName("segmentation");



        auto resizeManip = pipeline->create<dai::node::ImageManip>();
        resizeManip->initialConfig.setResize(previewSize.x, previewSize.y);
        resizeManip->initialConfig.setFrameType(dai::ImgFrame::Type::BGR888p);


        std::tuple <int, int> resVid = camRgbNode->getCam()->getPreviewSize();
        float ratio = (float(std::get<0>(resVid)) - float(std::get<1>(resVid)) )/ float(std::get<0>(resVid));


        float ratioXMin = ratio / 2.;
        float ratioXMax = 1.0 - ratio / 2.;
        offsetCrop = { ratioXMin, ratioXMax };
        auto cropManip = pipeline->create<dai::node::ImageManip>();
        cropManip->initialConfig.setCropRect(ratioXMin, 0, ratioXMax, 1);
        cropManip->setMaxOutputFrameSize(camRgbNode->getCam()->getPreviewHeight() * camRgbNode->getCam()->getPreviewWidth() * 3);


        xin->setMaxDataSize(300 * 300 * 3);
        xin->setNumFrames(30);


        // Linking
        camRgbNode->getCam()->preview.link(cropManip->inputImage);
        cropManip->out.link(resizeManip->inputImage);
        resizeManip->out.link(detectionNNNode->getNN()->input);


        //camera to screen
        camRgbNode->getCam()->preview.link(xoutRgb->input);
        detectionNNNode->getNN()->passthrough.link(xoutPreview->input);
        
        xin->out.link(detectionNNNode->getNN()->input);

        // segmantation to Screen
        detectionNNNode->getNN()->out.link(nnOut->input);

        // Connect to device and start pipeline
        device = new dai::Device(*pipeline, dai::UsbSpeed::SUPER);


        qRgb = device->getOutputQueue("previewNN", 4, false);
        qNN = device->getOutputQueue("segmentation", 4, false);
        inDataInQueue = device->getInputQueue("nn_in");

        qCam = device->getOutputQueue("rgb", 4, false);

        tensor = std::make_shared<dai::RawBuffer>();
        tensorData = false;
	}

    void SemanticSegComponentInstance::update(double deltaTime)
    {

        if (qCam) {


            std::shared_ptr<dai::ImgFrame> inPreview = qRgb->tryGet<dai::ImgFrame>();
            std::shared_ptr<dai::NNData> inDet = qNN->tryGet<dai::NNData>();
            std::shared_ptr<dai::ImgFrame> inRgb = qCam->tryGet<dai::ImgFrame>();

            if( mOakFrame->texturesInitDone()){

                if (inRgb) {
                    cv::Mat colorFrame = inRgb->getCvFrame();
                    mOakFrame->updateSSMainTex(&colorFrame);
                }

                if (inPreview) {
                    cv::Mat previewFrame = inPreview->getCvFrame();
                    toPlanar(previewFrame, tensor->data);
                }

                if (tensorData)inDataInQueue->send(tensor);

               

               if (inDet) {
                    mOakFrame->updateSSMaskTex(inDet);
               }
                

            }
            else 
            {
                if (inRgb && inRgb->getWidth() != 0 && inRgb->getHeight() != 0) {
                    mOakFrame->initTextures(glm::vec2( inRgb->getWidth(), inRgb->getHeight() ), offsetCrop, previewSize);
                }
            }
        }

    }

    // from utilities of depthai-core/examples/utilities
    void SemanticSegComponentInstance::toPlanar(cv::Mat& bgr, std::vector<std::uint8_t>& data) {

        data.resize(bgr.cols * bgr.rows * 3);
        for (int y = 0; y < bgr.rows; y++) {
            for (int x = 0; x < bgr.cols; x++) {
                auto p = bgr.at<cv::Vec3b>(y, x);
                data[x + y * bgr.cols + 0 * bgr.rows * bgr.cols] = p[0];
                data[x + y * bgr.cols + 1 * bgr.rows * bgr.cols] = p[1];
                data[x + y * bgr.cols + 2 * bgr.rows * bgr.cols] = p[2];
            }
        }

        // first pass done 
        tensorData = true;
    }


}
