#include "depthaicoresemanticsegexampleapp.h"

// External Includes
#include <utility/fileutils.h>
#include <nap/logger.h>
#include <inputrouter.h>
#include <rendergnomoncomponent.h>
#include <perspcameracomponent.h>
#include <orthocameracomponent.h>

RTTI_BEGIN_CLASS_NO_DEFAULT_CONSTRUCTOR(nap::DepthaiCoreSemanticSegExampleApp)
	RTTI_CONSTRUCTOR(nap::Core&)
RTTI_END_CLASS

namespace nap 
{
	/**
	 * Initialize all the resources and instances used for drawing
	 * slowly migrating all functionality to NAP
	 */
	bool DepthaiCoreSemanticSegExampleApp::init(utility::ErrorState& error)
	{
		// Retrieve services
		mRenderService = getCore().getService<nap::RenderService>();
		mSceneService = getCore().getService<nap::SceneService>();
		mInputService = getCore().getService<nap::InputService>();
		mGuiService = getCore().getService<nap::IMGuiService>();


		mDepthAICoreService = getCore().getService<nap::DepthAICoreService>();

		//mDepthAiCoreService->initFrame();
		// Fetch the resource manager
		mResourceManager = getCore().getResourceManager();


		// Get the render window
		mRenderWindow = mResourceManager->findObject<nap::RenderWindow>("Window");
		if (!error.check(mRenderWindow != nullptr, "unable to find render window with name: %s", "Window"))
			return false;

		// Get the scene that contains our entities and components
		mScene = mResourceManager->findObject<Scene>("Scene");
		if (!error.check(mScene != nullptr, "unable to find scene with name: %s", "Scene"))
			return false;

		mBackgroundEntity = mScene->findEntity("BackgroundEntity");
		if (!error.check(mBackgroundEntity != nullptr, "unable to find origin entity with name: %s", "BackgroundEntity"))
			return false;

		mOrthoCameraEntity = mScene->findEntity("OrthoCamEntity");
		if (!error.check(mOrthoCameraEntity != nullptr, "unable to find ortho cam entity with name: %s", "OrthCameraEntity"))
			return false;

		mOakCameraEntity = mScene->findEntity("OakCameraEntity");
		if (!error.check(mOakCameraEntity != nullptr, "unable to find camera holder entity with name: %s", "OakCameraEntity"))
			return false;

		OakCamRenderer = &mOakCameraEntity->getComponent< RenderOakCameraComponentInstance >();


		// All done!
		return true;
	}


	// Called when the window is updating
	void DepthaiCoreSemanticSegExampleApp::update(double deltaTime)
	{
		// Use a default input router to forward input events (recursively) to all input components in the default scene
		nap::DefaultInputRouter input_router(true);
		mInputService->processWindowEvents(*mRenderWindow, input_router, { &mScene->getRootEntity() });

		positionBackground();

	}


	// Called when the window is going to render
	void DepthaiCoreSemanticSegExampleApp::render()
	{
		// Signal the beginning of a new frame, allowing it to be recorded.
		// The system might wait until all commands that were previously associated with the new frame have been processed on the GPU.
		// Multiple frames are in flight at the same time, but if the graphics load is heavy the system might wait here to ensure resources are available.

		mRenderService->beginFrame();

		if (mRenderService->beginHeadlessRecording())
		{
			OakCamRenderer->draw();
			mRenderService->endHeadlessRecording();
		}
		// Begin recording the render commands for the main render window
		if (mRenderService->beginRecording(*mRenderWindow))
		{
			// Begin render pass
			mRenderWindow->beginRendering();

			// Render background plane
			mRenderService->renderObjects(*mRenderWindow, mOrthoCameraEntity->getComponent<OrthoCameraComponentInstance>(), { &mBackgroundEntity->getComponent<RenderableMeshComponentInstance>() });

			// Render GUI elements
			mGuiService->draw();

			// Stop render pass
			mRenderWindow->endRendering();

			// End recording
			mRenderService->endRecording();
		}

		// Proceed to next frame
		mRenderService->endFrame();
	}
	

	void DepthaiCoreSemanticSegExampleApp::windowMessageReceived(WindowEventPtr windowEvent)
	{
		mRenderService->addEvent(std::move(windowEvent));
	}
	
	
	void DepthaiCoreSemanticSegExampleApp::inputMessageReceived(InputEventPtr inputEvent)
	{
		if (inputEvent->get_type().is_derived_from(RTTI_OF(nap::KeyPressEvent)))
		{
			// If we pressed escape, quit the loop
			nap::KeyPressEvent* press_event = static_cast<nap::KeyPressEvent*>(inputEvent.get());
			if (press_event->mKey == nap::EKeyCode::KEY_ESCAPE)
				quit();

			// f is pressed, toggle full-screen
			if (press_event->mKey == nap::EKeyCode::KEY_f)
				mRenderWindow->toggleFullscreen();
		}
		// Add event, so it can be forwarded on update
		mInputService->addEvent(std::move(inputEvent));
	}

	
	int DepthaiCoreSemanticSegExampleApp::shutdown()
	{
		return 0;
	}

	void DepthaiCoreSemanticSegExampleApp::positionBackground() {

		float window_width = static_cast<float>(mRenderWindow->getWidthPixels());
		float window_heigh = static_cast<float>(mRenderWindow->getHeightPixels());

		// Calculate ratio
		float video_ratio = static_cast<float>(OakCamRenderer->getOakFrameSize().x) / static_cast<float>(OakCamRenderer->getOakFrameSize().y);
		float window_ratio = window_width / window_heigh;

		glm::vec3 plane_size = { window_width, window_heigh, 1.0 };
		if (window_ratio < video_ratio)
			plane_size.y = plane_size.x / video_ratio;
		else
			plane_size.x = plane_size.y * video_ratio;

		// Calculate plane offset in pixel coordinates
		glm::vec2 offset = { window_width / 2, window_heigh / 2 };

		// Get transform and push
		TransformComponentInstance& transform = mBackgroundEntity->getComponent<TransformComponentInstance>();
		transform.setTranslate(glm::vec3(offset.x, offset.y, 0.0f));
		transform.setScale(plane_size);
	}

}
