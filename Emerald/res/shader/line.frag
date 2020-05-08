#version 330

in vec2 fsUv;
in vec4 fsColor;

out vec3 outColor;

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
	outColor = vec3(fsColor);
	//outColor[1] = vec3(0.0, 0.0, 0.0);
}