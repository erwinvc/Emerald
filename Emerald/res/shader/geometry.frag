#version 430 core

struct Data {
	vec3 pos;
	vec3 normal;
	vec2 uv;
	mat3 TBNMatrix;
	vec3 viewDirection;
};

in Data fsData;

uniform sampler2D _Albedo;
uniform sampler2D _Normal;
uniform float _NormalStrength;
uniform sampler2D _Specular;
uniform float _SpecularStrength;
uniform sampler2D _Emission;
uniform float _EmissionStrength;

uniform sampler2D _Iridescence;
uniform float _IridescenceStrength;

out vec3 geoData[4];

void main(){
	vec4 albedo = texture(_Albedo, fsData.uv).rgba;
	if (albedo.a < 0.2) discard;
	

	vec3 tangentNormal = (texture(_Normal, fsData.uv).xyz * 2.0f) - 1.0f;
    vec3 worldNormal = normalize(fsData.TBNMatrix * tangentNormal);
	vec3 finalNormal = mix(fsData.normal, worldNormal, _NormalStrength);

	vec3 nView = normalize(fsData.viewDirection);
	vec3 nReflection = normalize(reflect(nView, finalNormal)); 
    float inverseDotView = 1.0 - max(dot(normalize(finalNormal), nView), 0.0);
    vec3 iridescence = texture(_Iridescence, vec2(inverseDotView, 0.0)).rgb;

	float specular = texture(_Specular, fsData.uv).x * _SpecularStrength;
	float emission = texture(_Emission, fsData.uv).x * _EmissionStrength;

	geoData[0] = vec3(specular, 0, emission);
	geoData[1] = albedo.rgb * mix(vec3(1), iridescence, _IridescenceStrength);
	geoData[2] = finalNormal;
	geoData[3] = fsData.pos;
}


