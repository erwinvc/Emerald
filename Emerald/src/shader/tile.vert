#version 330
layout(location = 0) in vec3 vsPos;
layout(location = 1) in vec3 vsNormal;
layout(location = 2) in vec2 vsUv;
layout(location = 3) in vec3 vsTangents;
layout(location = 4) in vec3 vsBitangents;
layout(location = 5) in vec2 vsPosition;

out vec3 fsPos;
out vec3 fsNormal;
out vec2 fsUv;
out mat3 tbnMatrix;
out vec3 fstangent;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;

void main()
{
	vec3 tangent = normalize(vsTangents - dot(vsNormal, vsTangents) * vsNormal);
	if (dot(cross(vsNormal, tangent), vsBitangents) < 0.0f) tangent *= -1.0f;
    tbnMatrix = mat3(tangent, vsBitangents, vsNormal);

	mat3 scale = mat3(
	0.01f, 0, 0, 0, 0.01f, 0, 0, 0, 0.01f
	);

	vec3 pos = vsPos * scale;
	fsPos = vec3(pos.x + vsPosition.x, pos.y, pos.z + vsPosition.y);

	fsNormal = vsNormal;
	fsUv = vsUv;
	gl_Position = projectionMatrix * viewMatrix * vec4(fsPos, 1.0);
}