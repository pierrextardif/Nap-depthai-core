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
    bool SemanticSegComponentInstance::init(utility::ErrorState& errorState)
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
        auto xoutRgb = pipeline->create<dai::node::XLinkOut>();
        auto nnOut = pipeline->create<dai::node::XLinkOut>();

        xoutRgb->setStreamName("rgb");
        nnOut->setStreamName("segmentation");



        auto resizeManip = pipeline->create<dai::node::ImageManip>();
        resizeManip->initialConfig.setResize(previewSize.x, previewSize.y);
        resizeManip->initialConfig.setFrameType(dai::ImgFrame::Type::BGR888p);


        std::tuple <int, int> resVid = camRgbNode->getCam()->getPreviewSize();
        float ratio = (float(std::get<0>(resVid)) - float(std::get<1>(resVid))) / float(std::get<0>(resVid));


        float ratioXMin = ratio / 2.;
        float ratioXMax = 1.0 - ratio / 2.;
        offsetCrop = { ratioXMin, ratioXMax };
        auto cropManip = pipeline->create<dai::node::ImageManip>();
        cropManip->initialConfig.setCropRect(ratioXMin, 0, ratioXMax, 1);
        cropManip->setMaxOutputFrameSize(camRgbNode->getCam()->getPreviewHeight() * camRgbNode->getCam()->getPreviewWidth() * 3);


        // Linking
        camRgbNode->getCam()->preview.link(cropManip->inputImage);
        cropManip->out.link(resizeManip->inputImage);
        resizeManip->out.link(detectionNNNode->getNN()->input);

        //camera to screen
        camRgbNode->getCam()->preview.link(xoutRgb->input);
        // segmantation to Screen
        detectionNNNode->getNN()->out.link(nnOut->input);

        // Connect to device and start pipeline
        device = new dai::Device(*pipeline, dai::UsbSpeed::SUPER);


        qNN = device->getOutputQueue("segmentation", 4, false);
        qCam = device->getOutputQueue("rgb", 4, false);
    }

    void SemanticSegComponentInstance::update(double deltaTime)
    {

        if (qCam) {

            std::shared_ptr<dai::ImgFrame> inRgb = qCam->tryGet<dai::ImgFrame>();
            std::shared_ptr<dai::NNData> inDet = qNN->tryGet<dai::NNData>();

            if (mOakFrame->texturesInitDone()) {

                if (inRgb) {
                    cv::Mat colorFrame = inRgb->getCvFrame();
                    mOakFrame->updateSSMainTex(&colorFrame);
                }

                if (inDet) {
                    mOakFrame->updateSSMaskTex(inDet);
                }


            }
            else
            {
                if (inRgb && inRgb->getWidth() != 0 && inRgb->getHeight() != 0) {
                    mOakFrame->initTextures(glm::vec2(inRgb->getWidth(), inRgb->getHeight()), offsetCrop, previewSize);
                }
            }
        }

    }
}