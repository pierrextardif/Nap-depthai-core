// Local Includes
#include "depthaicoreservice.h"

// External Includes
#include <nap/core.h>
#include <nap/resourcemanager.h>
#include <nap/logger.h>
#include <iostream>

RTTI_BEGIN_CLASS_NO_DEFAULT_CONSTRUCTOR(nap::DepthAICoreService)
	RTTI_CONSTRUCTOR(nap::ServiceConfiguration*)
RTTI_END_CLASS

namespace nap
{
	bool DepthAICoreService::init(nap::utility::ErrorState& errorState)
	{
		//Logger::info("Initializing DepthAICoreService");
		return true;
	}


	void DepthAICoreService::update(double deltaTime)
	{
	}


	std::shared_ptr <dai::Pipeline > DepthAICoreService::getPipeline()
	{
		return std::make_shared<dai::Pipeline>(pipeline);
	}

	void DepthAICoreService::getDependentServices(std::vector<rtti::TypeInfo>& dependencies)
	{
	}
	

	void DepthAICoreService::shutdown()
	{
	}

	void DepthAICoreService::registerObjectCreators(rtti::Factory& factory)
	{
		factory.addObjectCreator(std::make_unique<OakFrameRenderObjectCreator>(*this));
		factory.addObjectCreator(std::make_unique<ColorCameraNodeObjectCreator>(*this));
		factory.addObjectCreator(std::make_unique<NeuralNetworkNodeObjectCreator>(*this));
	}
}
