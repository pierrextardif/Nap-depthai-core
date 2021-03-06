#pragma once

// External Includes
#include <nap/device.h>
#include <nap/resourceptr.h>
#include <nap/numeric.h>
#include <texture2d.h>

#include <depthaicoreservice.h>

#include "depthai/depthai.hpp"

using namespace nap;

namespace nap
{
	class DepthAICoreService;




	class NAPAPI NeuralNetworkNode : public Device
	{
		RTTI_ENABLE(Device)
			friend class DepthAICoreService;


	public:

		NeuralNetworkNode(DepthAICoreService& service);
		virtual bool start(utility::ErrorState& errorState) override;
		virtual void stop() override;
		std::shared_ptr < dai::node::NeuralNetwork> getNN();
		DepthAICoreService& getDepthAICoreService();
		std::shared_ptr < dai::Pipeline > getPipelinePointer();
		glm::vec2 getSizeNN();

		std::string nnPath = "C:/Users/pierr/Documents/DEV/NAP/NAP-0.5.0-Win64-x86_64/user_modules/mod_depthaicore/data/nn/deeplabv3p_person_6_shaves.blob";
		int inferenceThreads = 1;
		bool blocking = false;
		glm::vec2 sizeNN = { 256, 256 };

	private:

		void initNN();

		DepthAICoreService& mService;

		std::shared_ptr < dai::Pipeline > pipeline;
		std::shared_ptr < dai::node::NeuralNetwork > detectionNN;
		


	};

	using NeuralNetworkNodeObjectCreator = rtti::ObjectCreator<NeuralNetworkNode, DepthAICoreService>;

}




