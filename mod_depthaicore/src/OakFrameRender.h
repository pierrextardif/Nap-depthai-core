#pragma once

// External Includes
#include <nap/device.h>
#include <nap/resourceptr.h>
#include <nap/numeric.h>
#include <texture2d.h>


#include "depthai/depthai.hpp"

using namespace nap;

namespace nap
{
	class OakFrameRender
	{
	public :
		
		OakFrameRender();
		~OakFrameRender();

		void setup();

	private :

		dai::Pipeline pipeline;
	};
}

