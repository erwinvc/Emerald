#version 330

#include "includes/globalUniforms.incl"

in vec3 vsPos;
in vec4 vars;
in vec4 col;

out vec4 fsPos;
out vec4 newPos;
out vec4 fsColor;

void main(){
	float uLightRadius = vars.w;
	vec3 uLightPosition = vars.xyz;
	vec4 pos = _Projection * _View * vec4((vsPos * uLightRadius) + uLightPosition, 1.0);
	gl_Position = pos;
	fsPos = pos;
	newPos = vars;
	fsColor = col;
}