// Local Includes
#include "depthaicoreservice.h"

// External Includes
#include <sceneservice.h>
#include <renderservice.h>
#include <nap/core.h>
#include <nap/resourcemanager.h>
#include <nap/logger.h>
#include <iostream>

RTTI_BEGIN_CLASS_NO_DEFAULT_CONSTRUCTOR(nap::DepthAiCoreService)
	RTTI_CONSTRUCTOR(nap::ServiceConfiguration*)
RTTI_END_CLASS

namespace nap
{
	bool DepthAiCoreService::init(nap::utility::ErrorState& errorState)
	{
		//Logger::info("Initializing DepthAiCoreService");
		return true;
	}


	void DepthAiCoreService::update(double deltaTime)
	{
	}
	

	void DepthAiCoreService::getDependentServices(std::vector<rtti::TypeInfo>& dependencies)
	{
		dependencies.emplace_back(RTTI_OF(SceneService));
		dependencies.emplace_back(RTTI_OF(RenderService));
	}
	

	void DepthAiCoreService::shutdown()
	{
	}

	void DepthAiCoreService::initFrame() {
		OakFrame = std::make_unique < OakFrameRender >();
	}
}
