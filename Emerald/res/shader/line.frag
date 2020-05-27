#version 330

#include "includes/globalUniforms.incl"

in vec2 fsUv;
in vec4 fsColor;

out vec4 outColor;

void main(){
	outColor = vec4(fsColor);
	//outColor[1] = vec3(0.0, 0.0, 0.0);
}