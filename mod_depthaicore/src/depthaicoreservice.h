#pragma once

#include <oakframerender.h>

// External Includes
#include <nap/service.h>


using namespace nap;

namespace nap
{
	class NAPAPI DepthAICoreService : public Service
	{
		friend class OakFrameRender;
		friend class TestFrameRender;
		RTTI_ENABLE(Service)
	public:
		// Default Constructor
		DepthAICoreService(ServiceConfiguration* configuration) : Service(configuration) { }

		/**
		 * Use this call to register service dependencies
		 * A service that depends on another service is initialized after all it's associated dependencies
		 * This will ensure correct order of initialization, update calls and shutdown of all services
		 * @param dependencies rtti information of the services this service depends on
		 */
		virtual void getDependentServices(std::vector<rtti::TypeInfo>& dependencies) override;

		/**
		 * Initializes the service
		 * @param errorState contains the error message on failure
		 * @return if the video service was initialized correctly
		 */
		virtual bool init(nap::utility::ErrorState& errorState) override;


		void initOak();
		/**
		 * Invoked by core in the app loop. Update order depends on service dependency
		 * This call is invoked after the resource manager has loaded any file changes but before
		 * the app update call. If service B depends on A, A:s:update() is called before B::update()
		 * @param deltaTime: the time in seconds between calls
		*/
		virtual void update(double deltaTime) override;

		/**
		 * Invoked when exiting the main loop, after app shutdown is called
		 * Use this function to close service specific handles, drivers or devices
		 * When service B depends on A, Service B is shutdown before A
		 */
		virtual void shutdown() override;
		/**
		* Registers a frame renderer with the service
		*/
		void registerOakFrame(nap::OakFrameRender& nFrame);

		/**
		* Removes a frame renderer from the service
		*/
		void removeOakFrameRender(nap::OakFrameRender& nFrame);

		glm::vec2 getSizeFrame();


	protected:
		virtual void registerObjectCreators(rtti::Factory& factory) override;


	private:

		std::vector< nap::OakFrameRender* > mOakFrames;

	};
}
