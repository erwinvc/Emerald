#version 330

layout(location = 0) in vec3 vsPos;
layout(location = 1) in vec3 vsNormal;
layout(location = 2) in vec2 vsUv;
layout(location = 3) in vec3 vsTangents;

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
	vec3 tangent = normalize(vsTangents - dot(vsNormal, vsTangents) * vsNormal);
	vec3 biTangent = cross(vsNormal, tangent);

	fsData.normal = (_TransformationMatrix * vec4(vsNormal, 0.0)).xyz;
    fsData.TBNMatrix = mat3((_TransformationMatrix * vec4(tangent, 0.0)).xyz, (_TransformationMatrix * vec4(biTangent, 0.0)).xyz, fsData.normal);

	fsData.pos = (_TransformationMatrix * vec4(vsPos, 1.0)).xyz;
	fsData.uv = vec2(vsUv.x, -vsUv.y);

	fsData.viewDirection = _CameraPosition - fsData.pos;
	gl_Position = _ProjectionMatrix * _ViewMatrix * vec4(fsData.pos, 1.0);
}

