#version 330
layout(location = 0) in vec3 vsPos;
layout(location = 1) in vec4 vsColor;

out vec4 fsColor;

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
	fsColor = vsColor;
    gl_Position = _Projection * _View * vec4(vsPos, 1.0);
}