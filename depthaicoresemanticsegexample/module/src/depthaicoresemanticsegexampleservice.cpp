// Local Includes
#include "depthaicoresemanticsegexampleservice.h"

// External Includes
#include <nap/core.h>
#include <nap/resourcemanager.h>
#include <nap/logger.h>
#include <iostream>

RTTI_BEGIN_CLASS_NO_DEFAULT_CONSTRUCTOR(nap::DepthaiCoreSemanticSegExampleService)
	RTTI_CONSTRUCTOR(nap::ServiceConfiguration*)
RTTI_END_CLASS

namespace nap
{
	bool DepthaiCoreSemanticSegExampleService::init(nap::utility::ErrorState& errorState)
	{
		//Logger::info("Initializing DepthaiCoreSemanticSegExampleService");
		return true;
	}


	void DepthaiCoreSemanticSegExampleService::update(double deltaTime)
	{
	}
	

	void DepthaiCoreSemanticSegExampleService::getDependentServices(std::vector<rtti::TypeInfo>& dependencies)
	{
	}
	

	void DepthaiCoreSemanticSegExampleService::shutdown()
	{
	}
}
