#version 330
in vec2 fsUV;

out vec3 outColor[2];

uniform sampler2D _GMisc;
uniform sampler2D _GAlbedo;

uniform float _BloomFactor;
uniform float _AmbientIntensity;

void main(){
	vec3 albedo = texture(_GAlbedo, fsUV).xyz;
	float emission = texture(_GMisc, fsUV).z * 2;
	//if(emission == 0) discard;


	outColor[0] = vec3(albedo * emission + albedo * _AmbientIntensity);
	outColor[1] = max(outColor[0] - _BloomFactor, 0.0f);
}