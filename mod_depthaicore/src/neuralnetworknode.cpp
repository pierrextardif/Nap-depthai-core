#include "neuralnetworknode.h"

#include <depthaicoreservice.h>
//external includes
#include <mathutils.h>
#include <nap/assert.h>
#include <rtti/typeinfo.h>


RTTI_BEGIN_CLASS_NO_DEFAULT_CONSTRUCTOR(nap::NeuralNetworkNode)
RTTI_PROPERTY_FILELINK("Neural Network Path", &nap::NeuralNetworkNode::nnPath, nap::rtti::EPropertyMetaData::Required, nap::rtti::EPropertyFileType::Any)
RTTI_PROPERTY("inference Thread", &nap::NeuralNetworkNode::inferenceThreads, nap::rtti::EPropertyMetaData::Required);
RTTI_PROPERTY("blocking", &nap::NeuralNetworkNode::blocking, nap::rtti::EPropertyMetaData::Required);
RTTI_PROPERTY("size of Neural Network Input", &nap::NeuralNetworkNode::sizeNN, nap::rtti::EPropertyMetaData::Required);
RTTI_END_CLASS

//////////////////////////////////////////////////////////////////////////


namespace nap
{
	NeuralNetworkNode::NeuralNetworkNode(DepthAICoreService& service):
		mService(service)
	{
	}

	bool NeuralNetworkNode::start(utility::ErrorState& errorState) 
	{
		pipeline = mService.getPipeline();
		initNN();

		return true;
	}
	
	DepthAICoreService& NeuralNetworkNode::getDepthAICoreService()
	{
		return mService;
	}

	void NeuralNetworkNode::initNN()
	{
		detectionNN = pipeline->create<dai::node::NeuralNetwork>();
		detectionNN->setBlobPath(nnPath);
		detectionNN->setNumInferenceThreads(inferenceThreads);
		detectionNN->input.setBlocking(blocking);


	}

	std::shared_ptr < dai::node::NeuralNetwork> NeuralNetworkNode::getNN()
	{
		return detectionNN;
	}

	std::shared_ptr < dai::Pipeline > NeuralNetworkNode::getPipelinePointer()
	{
		return pipeline;
	}

	void NeuralNetworkNode::stop()
	{

	}

	glm::vec2 NeuralNetworkNode::getSizeNN()
	{
		return sizeNN;
	}
}
