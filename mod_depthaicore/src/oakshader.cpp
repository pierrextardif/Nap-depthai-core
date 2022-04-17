/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

 // Local includes
#include "oakshader.h"
#include "renderservice.h"

// External includes
#include <nap/core.h>

// nap::CameraShader run time class definition 
RTTI_BEGIN_CLASS_NO_DEFAULT_CONSTRUCTOR(nap::OakShader)
RTTI_CONSTRUCTOR(nap::Core&)
RTTI_END_CLASS



//////////////////////////////////////////////////////////////////////////
// CameraShader
//////////////////////////////////////////////////////////////////////////

namespace nap
{
	namespace oakVideo {
		inline constexpr const char* pass = "pass";
	}
	OakShader::OakShader(Core& core) : Shader(core),
	mRenderService(core.getService<RenderService>(){ }


	bool OakShader::init(utility::ErrorState& errorState)
	{
		//  !!naprender data folder
		return loadDefault(oakVideo::pass, errorState);
	}
}