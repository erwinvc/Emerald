#version 430 core

layout(location = 0) in vec3 vsPos;
layout(location = 1) in vec3 vsNormal;
layout(location = 2) in vec2 vsUv;
layout(location = 3) in vec3 vsBitangents;
layout(location = 4) in vec3 vsTangents;

struct Data {
	vec4 pos;
	vec2 uv;
	vec3 normal;
	mat3 TBN;
};

out Data vsData;

uniform mat4 _ModelMatrix;
uniform mat4 _ViewMatrix;
uniform mat4 _ProjectionMatrix;

void main(){
	vsData.uv = vsUv;

	vec3 n = normalize((_ModelMatrix * vec4(vsNormal, 0.0)).xyz);
	vec3 t = (_ModelMatrix * vec4(vsTangents, 0.0)).xyz;

	t = normalize(t - dot(t, n) * n);
	vec3 b = cross(n, t);
	vsData.TBN = mat3(t, b, n);

	vsData.normal = mat3(transpose(inverse(_ModelMatrix))) * vsNormal;

	vec4 worldPos = _ModelMatrix * vec4(vsPos, 1.0);
	vsData.pos = worldPos;
	gl_Position = _ProjectionMatrix * _ViewMatrix * worldPos;
}