#version 330
layout(location = 0) in vec3 vsPos;
layout(location = 1) in vec3 vsNormal;
layout(location = 2) in vec2 vsUv;
layout(location = 3) in vec3 vsTangents;
layout(location = 4) in vec3 vsBitangents;
layout(location = 5) in vec2 vsPosition;
layout(location = 6) in float vsRotation;

out vec3 fsPos;
out vec3 fsNormal;
out vec2 fsUv;
out mat3 tbnMatrix;
out vec3 fstangent;
out vec3 v_view_direction;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;

uniform mat4 rotation1;
uniform mat4 rotation2;
uniform mat4 rotation3;
uniform mat4 rotation4;


uniform sampler2D texture_iridescence;
uniform sampler2D texture_noise;

void main()
{
	vec3 worldNormal = (rotation1 * vec4(vsNormal, 1.0)).xyz;
	vec3 worldPos = (rotation1 * vec4(vsPos, 1.0)).xyz;
	vec3 worldTangent = (rotation1 * vec4(vsTangents, 1.0)).xyz;
	vec3 worldBiTangent = (rotation1 * vec4(vsBitangents, 1.0)).xyz;

	vec3 tangent = normalize(worldTangent - dot(worldNormal, worldTangent) * worldNormal);
	if (dot(cross(worldNormal, tangent), worldBiTangent) < 0.0f) tangent *= -1.0f;
    tbnMatrix = mat3(tangent, worldBiTangent, worldNormal);

	//mat3 scale = mat3(
	//0.01f, 0, 0, 0, 0.01f, 0, 0, 0, 0.01f
	//);

	//vec3 pos = vsPos * scale;

	fsPos = vec3(worldPos.x + vsPosition.x + 0.5f, worldPos.y, worldPos.z + vsPosition.y + 0.5f);

	fsNormal = worldNormal;
	fsUv = vsUv;

	v_view_direction = (inverse(viewMatrix) * vec4(0.0, 0.0, 0.0, 1.0)).xyz - fsPos;
	gl_Position = projectionMatrix * viewMatrix * vec4(fsPos, 1.0);
}