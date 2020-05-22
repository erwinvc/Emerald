#version 330 core

in vec3 vsPos;
in vec2 vsIndex; //x = vertex; y = block side;

uniform vec3 _ChunkPos;

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

void main()
{
    vec3 pos = vsPos + _ChunkPos;
    gl_Position = vec4(pos, 1.0);
}  