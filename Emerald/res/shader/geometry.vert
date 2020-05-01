#version 330

layout(location = 0) in vec3 vsPos;
layout(location = 1) in vec3 vsNormal;
layout(location = 2) in vec2 vsUv;
layout(location = 3) in vec3 vsTangent;
layout(location = 4) in vec3 vsBitangent;

struct Data {
	vec3 normal;
	vec2 uv;
	mat3 TBNMatrix;
	vec3 viewDirection;
};

out Data fsData;

layout (std140) uniform GlobalUniforms {
	vec3 _CameraPosition;
	mat4 _Projection;
	mat4 _View;
	mat4 _InverseProjection;
	mat4 _InverseView;
    float _BloomFactor;
    bool _SSAOEnabled;
};

uniform mat4 _TransformationMatrix;

void main(){
	mat4 MVMatrix = _View * _TransformationMatrix;

	vec4 viewPos = MVMatrix * vec4(vsPos, 1.0);

	vec3 n = normalize((_TransformationMatrix * vec4(vsNormal, 0.0)).xyz);
	vec3 t = (_TransformationMatrix * vec4(vsTangent, 0.0)).xyz;
	t = normalize(t - dot(t, n) * n);
	vec3 b = cross(n, t);
	fsData.TBNMatrix = mat3(t, b, n);
	fsData.normal = n;

	fsData.uv = vsUv;
	fsData.viewDirection = _CameraPosition - viewPos.xyz;

	gl_Position = _Projection * _View * _TransformationMatrix * vec4(vsPos, 1.0);
}