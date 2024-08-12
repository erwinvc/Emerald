#version 430 core

layout(location = 0) in vec3 vsPos;
layout(location = 1) in vec2 vsUv;

struct Data {
	vec3 pos;
	vec2 uv;
};

out Data vsData;

void main(){
	vsData.pos =(vec4(vsPos, 1.0)).xyz;
	vsData.uv = vsUv;
	gl_Position = vec4(vsData.pos, 1.0);
}

