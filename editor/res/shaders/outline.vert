#version 430 core

layout(location = 0) in vec3 vsPos;
layout(location = 1) in vec3 vsNormal;
layout(location = 2) in vec2 vsUv;
layout(location = 3) in vec3 vsBitangents;
layout(location = 4) in vec3 vsTangents;

uniform mat4 _ModelMatrix;
uniform mat4 _ViewMatrix;
uniform mat4 _ProjectionMatrix;

void main(){
	vec3 pos = vsPos;
	pos += vsNormal * 0.15;
	gl_Position = _ProjectionMatrix * _ViewMatrix * _ModelMatrix * vec4(pos, 1.0);
}