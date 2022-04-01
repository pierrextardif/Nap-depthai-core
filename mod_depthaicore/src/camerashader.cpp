/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

 // Local includes
#include "camerashader.h"

// External includes
#include <nap/core.h>

// nap::CameraShader run time class definition 
RTTI_BEGIN_CLASS_NO_DEFAULT_CONSTRUCTOR(nap::CameraShader)
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
uniform sampler2D yTexture;
uniform sampler2D uTexture;
uniform sampler2D vTexture;
in vec3 pass_Uvs;

const vec3 R_cf = vec3(1.164383,  0.000000,  1.596027);
const vec3 G_cf = vec3(1.164383, -0.391762, -0.812968);
const vec3 B_cf = vec3(1.164383,  2.017232,  0.000000);
const vec3 offset = vec3(-0.0625, -0.5, -0.5);
out vec4 out_Color;

void main() 
{
  float y = texture(yTexture, vec2(pass_Uvs.x, 1.0-pass_Uvs.y)).r;
  float u = texture(uTexture, vec2(pass_Uvs.x, 1.0-pass_Uvs.y)).r;
  float v = texture(vTexture, vec2(pass_Uvs.x, 1.0-pass_Uvs.y)).r;
  vec3 yuv = vec3(y,u,v);
  yuv += offset;
  out_Color = vec4(0.0, 0.0, 0.0, 1.0);
  out_Color.r = dot(yuv, R_cf);
  out_Color.g = dot(yuv, G_cf);
  out_Color.b = dot(yuv, B_cf);
})glslang";


//////////////////////////////////////////////////////////////////////////
// CameraShader
//////////////////////////////////////////////////////////////////////////

namespace nap
{
	CameraShader::CameraShader(Core& core) : Shader(core) { }


	bool CameraShader::init(utility::ErrorState& errorState)
	{
		// Number of characters = number of bytes minus null termination character of string literal.
		auto vert_size = sizeof(camVertShader) - 1;
		auto frag_size = sizeof(camFragShader) - 1;
		return load("CameraShader", camVertShader, vert_size, camFragShader, frag_size, errorState);
	}
}