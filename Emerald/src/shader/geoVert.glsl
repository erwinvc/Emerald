#version 330
layout(location = 0) in vec3 vsPos;
layout(location = 1) in vec3 vsNormal;
layout(location = 2) in vec2 vsUv;
layout(location = 3) in vec3 vsTangents;
layout(location = 4) in vec3 vsBitangents;

out vec3 fsPos;
out vec3 fsNormal;
out vec2 fsUv;
out mat3 tbnMatrix;
out vec3 fstangent;
out vec3 v_view_direction;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;

uniform sampler2D texture_iridescence;
uniform sampler2D texture_noise;

void main()
{
	vec3 tangent = normalize(vsTangents - dot(vsNormal, vsTangents) * vsNormal);
	if (dot(cross(vsNormal, tangent), vsBitangents) < 0.0f) tangent *= -1.0f;
    tbnMatrix = mat3(tangent, vsBitangents, vsNormal);

	//mat3 scale = mat3(
	//0.01f, 0, 0, 0, 0.01f, 0, 0, 0, 0.01f
	//);

	//vec3 pos = vsPos * scale;
	fsPos = vsPos;

	fsNormal = vsNormal;
	fsUv = vsUv;

	v_view_direction = (inverse(viewMatrix) * vec4(0.0, 0.0, 0.0,1.0)).xyz - fsPos;
	gl_Position = projectionMatrix * viewMatrix * vec4(fsPos, 1.0);
}