/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

 // Local includes
#include "oakshader.h"

// External includes
#include <nap/core.h>

// nap::CameraShader run time class definition 
RTTI_BEGIN_CLASS_NO_DEFAULT_CONSTRUCTOR(nap::OakShader)
RTTI_CONSTRUCTOR(nap::Core&)
RTTI_END_CLASS

//////////////////////////////////////////////////////////////////////////
// Video Vertex Shader
//////////////////////////////////////////////////////////////////////////

static const char camVertShader[] = R"glslang(
#version 450 core
uniform nap
{
	mat4 projectionMatrix;
	mat4 viewMatrix;
	mat4 modelMatrix;
} mvp;

in vec3	in_Position;
in vec3	in_UV0;
out vec3 pass_Uvs;

void main(void)
{
    gl_Position = mvp.projectionMatrix * mvp.viewMatrix * mvp.modelMatrix * vec4(in_Position, 1.0);
	pass_Uvs = in_UV0;
})glslang";


//////////////////////////////////////////////////////////////////////////
// Video Fragment Shader
//////////////////////////////////////////////////////////////////////////

static const char camFragShader[] = R"glslang(
#version 450 core
uniform sampler2D colorTexture;
in vec3 pass_Uvs;

out vec4 out_Color;

void main() 
{
  out_Color = vec4(1., 0., 0., 1.); 
})glslang";

//texture(colorTexture, pass_Uvs.xy);



//////////////////////////////////////////////////////////////////////////
// CameraShader
//////////////////////////////////////////////////////////////////////////

namespace nap
{
	OakShader::OakShader(Core& core) : Shader(core) { }


	bool OakShader::init(utility::ErrorState& errorState)
	{
		// Number of characters = number of bytes minus null termination character of string literal.
		auto vert_size = sizeof(camVertShader) - 1;
		auto frag_size = sizeof(camFragShader) - 1;
		return load("OakShader", camVertShader, vert_size, camFragShader, frag_size, errorState);
	}
}