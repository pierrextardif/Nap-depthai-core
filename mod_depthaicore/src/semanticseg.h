#pragma once

// External Includes
#include <component.h>
#include <componentptr.h>
#include <entity.h>

#include "depthai/depthai.hpp"
#include <colorcameranode.h>
#include <neuralnetworknode.h>
#include <oakframerender.h>

using namespace nap;

namespace nap
{
	class DepthAICoreService;
	class SemanticSegComponentInstance;


	class NAPAPI SemanticSegComponent : public Component
	{
		RTTI_ENABLE(Component)

		DECLARE_COMPONENT(SemanticSegComponent, SemanticSegComponentInstance)

	public:
		
		ResourcePtr < ColorCameraNode >		camRgb;
		ResourcePtr < NeuralNetworkNode >	detectionNN;
		ResourcePtr < OakFrameRender >		mOakFrame;
	};

	class NAPAPI SemanticSegComponentInstance : public ComponentInstance
	{
		RTTI_ENABLE(ComponentInstance)
	public:
		SemanticSegComponentInstance(EntityInstance& entity, Component& resource) :
			ComponentInstance(entity, resource) { }

		virtual bool init(utility::ErrorState& errorState) override;
		void update(double deltaTime);

		glm::vec2 getSizeNN();
	private :
		void initDAI();

		std::shared_ptr < dai::Pipeline > pipeline;
		dai::Device* device;


		std::shared_ptr<dai::DataOutputQueue>		qCam = nullptr;
		std::shared_ptr<dai::DataOutputQueue>		qNN = nullptr;


		glm::vec2 offsetCrop;
		glm::vec2 previewSize;
		ColorCameraNode* camRgbNode = nullptr;
		NeuralNetworkNode* detectionNNNode = nullptr;
		OakFrameRender* mOakFrame = nullptr;
	};

}




