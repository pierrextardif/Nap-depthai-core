#include "semanticseg.h"


#include <entity.h>
#include <algorithm>


RTTI_BEGIN_CLASS(nap::SemanticSegComponent)
RTTI_PROPERTY("color Cam Node", &nap::SemanticSegComponent::camRgb, nap::rtti::EPropertyMetaData::Required);
RTTI_PROPERTY("Nueral Network Node", &nap::SemanticSegComponent::detectionNN, nap::rtti::EPropertyMetaData::Required);
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
        previewSize = camRgbNode->getPreviewSize();
        
        initDAI();


        return true;
    }



    glm::vec2 SemanticSegComponentInstance::getSizeNN()
    {
        return previewSize;
    }

    void SemanticSegComponentInstance::initDAI()
	{


        pipeline.setOpenVINOVersion(dai::OpenVINO::VERSION_2021_4);

        // Define source and output
        auto xin = pipeline.create<dai::node::XLinkIn>();
        auto xoutRGB = pipeline.create<dai::node::XLinkOut>();
        auto nnOut = pipeline.create<dai::node::XLinkOut>();


        xoutRGB->setStreamName("rgb");
        xoutRGB->getStreamName();
        nnOut->setStreamName("segmentation");

        xin->setStreamName("nn_in");
        nnOut->setStreamName("segmentation");



        xin->setMaxDataSize(300 * 300 * 3);
        xin->setNumFrames(30);

        camRgbNode->getCam()->preview.link(detectionNNNode->getNN()->input);
        detectionNNNode->getNN()->passthrough.link(xoutRGB->input);

        // Linking
        //camRgb->video.link(xoutRGB->input);

        xin->out.link(detectionNNNode->getNN()->input);


        detectionNNNode->getNN()->out.link(nnOut->input);

        // Connect to device and start pipeline
        device = new dai::Device(pipeline);


        qRgb = device->getOutputQueue("rgb", 4, false);
        qNN = device->getOutputQueue("segmentation", 4, false);
        inDataInQueue = device->getInputQueue("nn_in");

        tensor = std::make_shared<dai::RawBuffer>();
	}

}
