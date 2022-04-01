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
		std::cout << "updating" << std::endl;
		for (auto* mO : mOakFrames) {
			mO->update(deltaTime);
		}
	}
	

	void DepthAiCoreService::getDependentServices(std::vector<rtti::TypeInfo>& dependencies)
	{
		dependencies.emplace_back(RTTI_OF(SceneService));
		dependencies.emplace_back(RTTI_OF(RenderService));
	}
	

	void DepthAiCoreService::shutdown()
	{
	}

	void DepthAiCoreService::registerObjectCreators(rtti::Factory& factory)
	{
		factory.addObjectCreator(std::make_unique<OakFrameRenderObjectCreator>(*this));
	}

	void DepthAiCoreService::registerOakFrame(nap::OakFrameRender& nFrame)
	{
		mOakFrames.emplace_back(&nFrame);
	}

	void DepthAiCoreService::removeOakFrameRender(nap::OakFrameRender& nFrame)
	{
		auto found_it = std::find_if(mOakFrames.begin(), mOakFrames.end(), [&](const auto& it)
			{
				return it == &nFrame;
			});
		assert(found_it != mOakFrames.end());
		mOakFrames.erase(found_it);
	}
}
