// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

#version 450 core

// vertex shader input  
in vec3 passUVs;						//< frag Uv's 

// uniform inputs
uniform sampler2D	textureOAK;

// output
out vec4 out_Color;

void main() 
{
	out_Color = texture(textureOAK, passUVs.xy);
}