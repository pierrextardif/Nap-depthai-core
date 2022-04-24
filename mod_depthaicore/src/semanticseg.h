#pragma once

// External Includes
#include <component.h>
#include <componentptr.h>
#include <entity.h>

#include "depthai/depthai.hpp"
#include <colorcameranode.h>
#include <neuralnetworknode.h>

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
		
		ResourcePtr < ColorCameraNode > camRgb;
		ResourcePtr < NeuralNetworkNode > detectionNN;
	};

	class NAPAPI SemanticSegComponentInstance : public ComponentInstance
	{
		RTTI_ENABLE(ComponentInstance)
	public:
		SemanticSegComponentInstance(EntityInstance& entity, Component& resource) :
			ComponentInstance(entity, resource) { }

		virtual bool init(utility::ErrorState& errorState) override;

		glm::vec2 getSizeNN();
	private :
		void initDAI();

		dai::Pipeline pipeline;
		dai::Device* device;


		std::shared_ptr<dai::DataOutputQueue> qRgb = nullptr;
		std::shared_ptr<dai::DataOutputQueue> qNN = nullptr;
		std::shared_ptr<dai::DataInputQueue> inDataInQueue = nullptr;

		std::shared_ptr<dai::RawBuffer> tensor = nullptr;

		glm::vec2 previewSize;
		ColorCameraNode* camRgbNode = nullptr;
		NeuralNetworkNode* detectionNNNode = nullptr;
	};

}




