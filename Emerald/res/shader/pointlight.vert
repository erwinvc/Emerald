#version 330

layout(location = 0) in vec3 vsPos;
layout(location = 1) in vec4 vars;
layout(location = 2) in vec4 col;
out vec4 fsPos;
out vec4 newPos;
out vec4 color;

uniform mat4 _Projection;
uniform mat4 _View;

//uniform float uLightRadius;
//uniform vec3 uLightPosition;

void main(){
	float uLightRadius = vars.w;
	vec3 uLightPosition = vars.xyz;
	vec4 pos = _Projection * _View * vec4((vsPos * uLightRadius) + uLightPosition, 1.0);

	gl_Position = pos;
	fsPos = pos;
	newPos = vars;
	color = col;
}