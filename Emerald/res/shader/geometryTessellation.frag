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
uniform sampler2D _Metallic;
uniform float _MetallicStrength;
uniform sampler2D _Emission;
uniform float _EmissionStrength;

uniform sampler2D _Iridescence;
uniform sampler2D _Noise;
uniform float _IridescenceStrength;

out vec4 geoData[4];

void main(){
	vec4 albedo = texture(_Albedo, fsData.uv).rgba;
	if (albedo.a < 0.2) discard;
	
	vec3 tangentNormal = (texture(_Normal, fsData.uv).xyz * 2.0f) - 1.0f;
    vec3 worldNormal = normalize(fsData.TBNMatrix * tangentNormal);
	vec3 finalNormal = mix(fsData.normal, worldNormal, _NormalStrength);

	float specular = texture(_Specular, fsData.uv).x * _SpecularStrength;
	float metallic = texture(_Metallic, fsData.uv).x * _MetallicStrength;
	float emission = texture(_Emission, fsData.uv).x * _EmissionStrength;

	//float mipmapLevel = textureQueryLod(_Albedo, fsData.uv).x / 7;

	vec3 noise = texture(_Noise, fsData.uv).rgb; 
	float fresnel = 1.0f - max(dot(normalize(fsData.viewDirection), finalNormal), 0.0f);

	vec3 iridescence = texture(_Iridescence, vec2(fresnel, 0) + noise.r).rgb;

	geoData[0] = vec4(specular, metallic, 0, 0);
	geoData[1] = vec4(albedo.rgb + (iridescence * noise), 1.0);
	geoData[2] = vec4(finalNormal, 1.0);
	geoData[3] = vec4(fsData.pos, 1.0);
}


