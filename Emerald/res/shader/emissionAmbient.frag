#version 330
in vec2 fsUV;

out vec3 outColor[2];

uniform sampler2D _GMisc;
uniform sampler2D _GAlbedo;
uniform sampler2D _SSAO;

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

uniform float _AmbientIntensity;

void main(){
	vec4 misc = texture(_GMisc, fsUV);
	vec3 albedo = texture(_GAlbedo, fsUV).xyz;
	float emission = misc.z * 2;
	float ssao = texture(_SSAO, fsUV).x;

	float lightInfluence = misc.w;
	//if(lightInfluence == 1) 
	//if(emission == 0) discard;

	outColor[0] = mix(albedo, vec3(albedo * emission + albedo * _AmbientIntensity) * ssao, lightInfluence);
	outColor[1] = max(outColor[0] - _BloomFactor, 0.0f);
}