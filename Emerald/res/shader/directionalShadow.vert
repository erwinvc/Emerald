#version 330 core
layout (location = 0) in vec3 vsPos;

uniform mat4 _LightSpaceMatrix;

uniform vec3 _ChunkPos;

uniform mat4 _TransformationMatrix;

layout (std140) uniform GlobalUniforms {
	vec3 _CameraPosition;
	mat4 _Projection;
	mat4 _View;
	mat4 _InverseProjection;
	mat4 _InverseView;
    bool _SSAOEnabled;
	vec2 _CameraPlanes;
	vec2 _ViewPort;
};

void main(){
    gl_Position = _LightSpaceMatrix * _TransformationMatrix * vec4(vsPos, 1.0);
}  