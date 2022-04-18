// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.
#version 450 core

uniform sampler2D colorTexture;

in vec3 pass_Uvs;

out vec4 out_Color;

void main() 
{
	vec2 uvs = pass_Uvs.xy;
	out_Color = texture(colorTexture, uvs);
}
