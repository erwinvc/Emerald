#version 430 core

layout(location = 0) in vec3 vsPos;
layout(location = 1) in vec3 vsNormal;
layout(location = 2) in vec2 vsUv;
layout(location = 3) in vec3 vsBitangents;
layout(location = 4) in vec3 vsTangents;

struct Data {
	vec4 pos;
	//vec2 uv;
	vec3 normal;
};

out Data vsData;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main(){
	//vsData.uv = vsUv;                                       // Pass UV coordinates
	vsData.normal = mat3(transpose(inverse(modelMatrix))) * vsNormal;  // Transform normal to world space

	vec4 worldPos = modelMatrix * vec4(vsPos, 1.0);         // Transform position to world space
	vsData.pos = worldPos;                                  // Pass world position to fragment shader
	gl_Position = projectionMatrix * viewMatrix * worldPos; // Calculate clip space position
}