#version 330
layout(location = 0) in vec3 vsPos;
layout(location = 1) in vec3 vsNormal;
layout(location = 2) in vec2 vsUv;
layout(location = 3) in vec3 vsTangents;
layout(location = 4) in vec3 vsBitangents;

out vec3 csPos;
out vec3 csNormal;
out vec2 csUv;
out mat3 csTBNMatrix;
out vec3 cstangent;
out vec3 csViewDirection;

uniform vec3 cameraPosition;
uniform mat4 transformationMatrix;
uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;

uniform sampler2D texture_iridescence;
uniform sampler2D texture_noise;

void main()
{
	vec3 tangent = normalize(vsTangents - dot(vsNormal, vsTangents) * vsNormal);
	//if (dot(cross(vsNormal, tangent), vsBitangents) < 0.0f) tangent *= -1.0f;
	vec3 biTangent = cross(vsNormal, tangent);
    csTBNMatrix = mat3(tangent, biTangent, vsNormal);

	//mat3 scale = mat3(
	//0.01f, 0, 0, 0, 0.01f, 0, 0, 0, 0.01f
	//);

	//vec3 pos = vsPos * scale;
	csPos = vsPos;
	csNormal = vsNormal;
	csUv = vsUv;

	csViewDirection = (inverse(viewMatrix) * vec4(0.0, 0.0, 0.0, 1.0)).xyz - csPos;
	gl_Position = vec4(csPos, 1.0);
}