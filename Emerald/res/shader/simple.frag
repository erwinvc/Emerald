#version 330
out vec3 outColor[2];
  
in vec2 fsUv;
  
uniform sampler2D _SSR;

layout (std140) uniform GlobalUniforms {
	vec3 _CameraPosition;
	mat4 _Projection;
	mat4 _View;
	mat4 _InverseProjection;
	mat4 _InverseView;
    float _BloomFactor;
    bool _SSAOEnabled;
	vec2 _CameraPlanes;
	vec2 _ViewPort;
};

void main() {
	vec3 col = texture(_SSR, fsUv).rgb;
	outColor[0] = col;
	outColor[1] = max(col - _BloomFactor, 0.0f);
}  