#version 330 core
in uvec2 vsData;

uniform mat4 _LightSpaceMatrix;

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

void main(){
	float x = float(vsData.x & 0x3FFu);
	float y = float((vsData.x & 0xFFC00u) >> 10u);
	float z = float((vsData.x & 0x3FF00000u) >> 20u);
	vec3 pos = vec3(x, y, z);
	pos += _ChunkPos;

	x += _ChunkPos.x;
	y += _ChunkPos.y;
	z += _ChunkPos.z;

    gl_Position = _LightSpaceMatrix * vec4(pos, 1.0);
}  