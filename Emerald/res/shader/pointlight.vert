#version 330

layout(location = 0) in vec3 vsPos;
layout(location = 1) in vec4 vars;
layout(location = 2) in vec4 col;
out vec4 fsPos;
out vec4 newPos;
out vec4 color;

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
	float uLightRadius = vars.w;
	vec3 uLightPosition = vars.xyz;
	vec4 pos = _Projection * _View * vec4((vsPos * uLightRadius) + uLightPosition, 1.0);

	gl_Position = pos;
	fsPos = pos;
	newPos = vars;
	color = col;
}