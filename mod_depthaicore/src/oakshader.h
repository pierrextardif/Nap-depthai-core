/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#pragma once

 // External Includes
#include <shader.h>

// Service include
#include <depthaicoreservice.h>

namespace nap
{
	// Forward declares
	class Core;
	class Material;

	// Video shader sampler names 
	namespace uniform
	{
		namespace oakVideo
		{
			inline constexpr const char* RGBASampler = "colorTexture";	///< video shader RGBA sampler name
		}
	}

	/**
	 * Used by the nap::RenderVideoComponent.
	 */
	class NAPAPI OakShader : public Shader
	{
		RTTI_ENABLE(Shader)
	public:
		OakShader(Core& core);

		/**
		 * Cross compiles the video GLSL shader code to SPIR-V, creates the shader module and parses all the uniforms and samplers.
		 * @param errorState contains the error if initialization fails.
		 * @return if initialization succeeded.
		 */
		virtual bool init(utility::ErrorState& errorState) override;

	private:
		RenderService* mRenderService = nullptr;
	};
}
