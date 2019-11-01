#version 330

layout(location = 0) in vec3 vsPos;
layout(location = 1) in vec3 vsNormal;
layout(location = 2) in vec2 vsUv;
layout(location = 3) in vec3 vsTangents;
layout(location = 4) in vec3 vsBitangents;
layout(location = 5) in vec2 vsPosition;
layout(location = 6) in vec2 vsRotation;
layout(location = 7) in float vsTextureID;

struct Data {
	vec3 pos;
	vec3 normal;
	vec2 uv;
	mat3 TBNMatrix;
	float textureID;
};
out Data fsData;

uniform mat4 _ProjectionMatrix;
uniform mat4 _ViewMatrix;

void main(){
    fsData.normal = vsNormal;
    fsData.TBNMatrix = mat3(vsTangents, vsBitangents, fsData.normal);

	vec4 viewPos = vec4(vsPos.x + (vsPosition.x * 10 + 5), vsPos.y, vsPos.z + (vsPosition.y * 10 + 5), 1.0);
	fsData.pos = viewPos.xyz;

	fsData.uv = vsUv;

	fsData.textureID = vsTextureID;
	gl_Position = _ProjectionMatrix * _ViewMatrix * viewPos;
}

