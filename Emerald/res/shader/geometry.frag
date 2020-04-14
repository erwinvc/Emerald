#version 330

struct Data {
	vec3 normal;
	vec2 uv;
	mat3 TBNMatrix;
	vec3 viewDirection;
};

in Data fsData;

uniform sampler2D _Albedo;
uniform sampler2D _Normal;
uniform float _NormalStrength;
uniform sampler2D _Roughness;
uniform float _RoughnessStrength;
uniform sampler2D _Metallic;
uniform float _MetallicStrength;
uniform sampler2D _Emission;
uniform float _EmissionStrength;
uniform float Iri;
uniform sampler2D _Iridescence;
uniform sampler2D _Noise;
uniform float _IridescenceStrength;
uniform mat4 _ViewMatrix;
out vec4 geoData[3];

void main(){
	vec4 albedo = texture(_Albedo, fsData.uv).rgba;
	if (albedo.a < 0.2) discard;
	
	//vec3 tangentNormal = (texture(_Normal, fsData.uv).xyz * 2.0f) - 1.0f;
    //vec3 worldNormal = normalize(fsData.TBNMatrix * tangentNormal);
	//vec3 finalNormal = mix(fsData.normal, worldNormal, 0);

	vec4 normal = vec4(fsData.TBNMatrix * (255.0/128.0 * texture(_Normal, fsData.uv).rgb - 1), 0);
    vec4 finalNormal = _ViewMatrix * normal;

	float roughness = texture(_Roughness, fsData.uv).x * _RoughnessStrength;
	float metallic = texture(_Metallic, fsData.uv).x * _MetallicStrength;
	float emission = texture(_Emission, fsData.uv).x * _EmissionStrength;

	//float mipmapLevel = textureQueryLod(_Albedo, fsData.uv).x / 7;

	vec3 noise = texture(_Noise, fsData.uv).rgb; 
	float fresnel = 1.0f - max(dot(normalize(fsData.viewDirection), finalNormal.xyz), 0.0f);
	vec3 iridescence = texture(_Iridescence, vec2(fresnel, 0) + noise.r).rgb;

	geoData[0] = vec4(roughness, metallic, 0, 1);
	geoData[1] = vec4(mix(albedo.rgb, iridescence * noise, Iri), 1.0);
	//geoData[1] = vec4((iridescence * noise), 1.0);
	geoData[2] = vec4(finalNormal);
}


