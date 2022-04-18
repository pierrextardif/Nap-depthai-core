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
		for (auto* mO : mOakFrames) {
			mO->update(deltaTime);
		}
	}
	
	void DepthAICoreService::initOak() {
		
		for (auto* mO : mOakFrames) {
			mO->init();
		}
	}

	void DepthAICoreService::getDependentServices(std::vector<rtti::TypeInfo>& dependencies)
	{
	}
	

	void DepthAICoreService::shutdown()
	{
	}


	glm::vec2 DepthAICoreService::getSizeFrame()
	{

		return mOakFrames.size() > 0 ? mOakFrames[0]->getOakFrameSize() : glm::vec2(1920, 1080);
	}



	void DepthAICoreService::registerObjectCreators(rtti::Factory& factory)
	{
		factory.addObjectCreator(std::make_unique<OakFrameRenderObjectCreator>(*this));
	}

	void DepthAICoreService::registerOakFrame(nap::OakFrameRender& nFrame)
	{
		mOakFrames.emplace_back(&nFrame);
	}

	void DepthAICoreService::removeOakFrameRender(nap::OakFrameRender& nFrame)
	{
		auto found_it = std::find_if(mOakFrames.begin(), mOakFrames.end(), [&](const auto& it)
			{
				return it == &nFrame;
			});
		assert(found_it != mOakFrames.end());
		mOakFrames.erase(found_it);
	}
}
