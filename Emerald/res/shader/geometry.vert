#version 330

layout(location = 0) in vec3 vsPos;
layout(location = 1) in vec3 vsNormal;
layout(location = 2) in vec2 vsUv;
layout(location = 3) in vec3 vsTangent;
layout(location = 4) in vec3 vsBitangent;

struct Data {
	vec3 pos;
	vec3 normal;
	vec2 uv;
	mat3 TBNMatrix;
	vec3 viewDirection;
};

out Data fsData;

uniform vec3 _CameraPosition;
uniform mat4 _TransformationMatrix;
uniform mat4 _ProjectionMatrix;
uniform mat4 _ViewMatrix;

void main(){
	//vec3 tangent = normalize(vsTangents - dot(vsNormal, vsTangents) * vsNormal);
	//vec3 biTangent = cross(vsNormal, tangent);

	mat3 normalMatrix = transpose(inverse(mat3(_TransformationMatrix)));
    fsData.normal = normalMatrix * vsNormal;
    fsData.TBNMatrix = mat3(normalMatrix * vsTangent, normalMatrix * vsBitangent, fsData.normal);

	vec4 viewPos = _TransformationMatrix * vec4(vsPos, 1.0);
	fsData.pos = viewPos.xyz;
	fsData.uv = vsUv;

	fsData.viewDirection = _CameraPosition - fsData.pos;
	gl_Position = _ProjectionMatrix * _ViewMatrix * viewPos;
}