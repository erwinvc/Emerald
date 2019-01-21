#version 330
layout(location = 0) in vec3 vsPos;
layout(location = 1) in vec3 vsNormal;
layout(location = 2) in vec2 vsUv;
layout(location = 3) in vec3 vsTangents;

out vec3 fsPos;
out vec3 fsNormal;
out vec2 fsUv;
out mat3 tbnMatrix;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;

void main()
{
	vec3 n = normalize(vec3(vec4(vsNormal, 0.0)));
	vec3 t = normalize(vec3(vec4(vsTangents, 0.0)));
	t = normalize(t - dot(t, n) * n);
	vec3 bitangent = cross(t, n);

	tbnMatrix = mat3(t, bitangent, n);
	tbnMatrix = transpose(tbnMatrix);

	fsPos = vsPos;
	fsNormal = vsNormal;
	fsUv = vsUv;
	gl_Position = projectionMatrix * viewMatrix * vec4(vsPos, 1.0);
}