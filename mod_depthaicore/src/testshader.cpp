/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

// Local includes
#include "testshader.h"
#include "renderservice.h"

// External includes
#include <nap/core.h>

// nap::VideoShader run time class definition 
RTTI_BEGIN_CLASS_NO_DEFAULT_CONSTRUCTOR(nap::TestShader)
	RTTI_CONSTRUCTOR(nap::Core&)
RTTI_END_CLASS


//////////////////////////////////////////////////////////////////////////
// VideoShader
//////////////////////////////////////////////////////////////////////////

namespace nap
{
	namespace shader
	{
		inline constexpr const char* video = "video";
	}

	TestShader::TestShader(Core& core) : Shader(core),
		mRenderService(core.getService<RenderService>()) { }


	bool TestShader::init(utility::ErrorState& errorState)
	{
		return loadDefault(shader::video, errorState);
	}
}
