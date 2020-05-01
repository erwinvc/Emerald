#version 330
layout(location = 0) in vec3 vsLineBegin;
layout(location = 1) in vec3 vsLineEnd;
layout(location = 2) in vec4 vsColor;

out vec3 fsPos;
out vec4 fsColor;

layout (std140) uniform GlobalUniforms {
	vec3 _CameraPosition;
	mat4 _Projection;
	mat4 _View;
	mat4 _InverseProjection;
	mat4 _InverseView;
    float _BloomFactor;
    bool _SSAOEnabled;
};

void main(){
	fsColor = vsColor;
    fsPos = gl_VertexID == 0 ? vsLineBegin : vsLineEnd;
    gl_Position = _Projection * _View * vec4(fsPos, 1.0);
}