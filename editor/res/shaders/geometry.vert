#version 430 core

layout(location = 0) in vec3 vsPos;
layout(location = 1) in vec2 vsUv;

struct Data {
	vec4 pos;
	vec2 uv;
};

out Data vsData;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main(){
	vsData.uv = vsUv;
	vsData.pos = projectionMatrix * viewMatrix * vec4(vsPos, 1.0);
	gl_Position = vsData.pos;
}