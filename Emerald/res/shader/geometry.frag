#version 330 //material

struct Data {
	vec3 normal;
	vec2 uv;
	mat3 TBNMatrix;
	vec3 viewDirection;
};

in Data fsData;

uniform vec4 _AlbedoColor;			//(COLOR, Albedo color) = (1.0, 0.0, 0.0, 0.0)
uniform sampler2D _Albedo;			//(TEXTURE, Albedo) = (null, 0)
uniform sampler2D _Normal;			//(TEXTURE, Normal) = (normal, 1)
uniform float _NormalStrength;		//(FLOAT, Normal strength) = (1.0, 0.0, 1.0)
uniform sampler2D _Roughness;		//(TEXTURE, Roughness) = (white, 2)
uniform float _RoughnessStrength;	//(FLOAT, Roughness strength) = (1.0, 0.0, 1.0)
uniform sampler2D _Metallic;		//(TEXTURE, Metallic) = (black, 3)
uniform float _MetallicStrength;	//(FLOAT, Metallic strength) = (1.0, 0.0, 1.0)
uniform sampler2D _Emission;		//(TEXTURE, Emission) = (white, 4)
uniform float _EmissionStrength;	//(FLOAT, Emission strength) = (0.0, 0.0, 1.0)

//uniform float Iri;					//(FLOAT, Iridescence) = (0.0, 0.0, 1.0)
//uniform sampler2D _Iridescence;		//(TEXTURE, Iridescence) = (black, 5)
//uniform sampler2D _Noise;			//(TEXTURE, Iridescence Noise) = (black, 6)
//uniform float _IridescenceStrength; //(FLOAT, Iridescence Strength) = (1.0, 0.0, 1.0)

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

out vec4 geoData[3];

void main(){
	vec4 albedo = texture(_Albedo, fsData.uv).rgba;
	if (albedo.a < 0.2) discard;
	//vec3 tangentNormal = (texture(_Normal, fsData.uv).xyz * 2.0f) - 1.0f;
    //vec3 worldNormal = normalize(fsData.TBNMatrix * tangentNormal);
	//vec3 finalNormal = mix(fsData.normal, worldNormal, 0);

	vec3 normalTex = (255.0/128.0 * (texture(_Normal, fsData.uv).rgb) - 1);
	vec3 normal = vec3(fsData.TBNMatrix * normalTex);
	normal = mix(fsData.normal, normal, _NormalStrength);
    vec4 finalNormal = normalize(_View * vec4(normal, 0.0));

	float roughness = texture(_Roughness, fsData.uv).x * _RoughnessStrength;
	float metallic = texture(_Metallic, fsData.uv).x * _MetallicStrength;
	float emission = texture(_Emission, fsData.uv).x * _EmissionStrength;

	//float mipmapLevel = textureQueryLod(_Albedo, fsData.uv).x / 7;

	//vec3 noise = texture(_Noise, fsData.uv).rgb; 
	//float fresnel = 1.0f - max(dot(normalize(fsData.viewDirection), finalNormal.xyz), 0.0f);
	//vec3 iridescence = texture(_Iridescence, vec2(fresnel, 0) + noise.r).rgb;

	geoData[0] = vec4(roughness, metallic, emission, 1);

	//geoData[1] = vec4(mix(albedo.rgb, iridescence * noise, Iri), 1.0);
	geoData[1] = vec4(mix(albedo.rgb, _AlbedoColor.rgb, _AlbedoColor.a), 1.0);
	//geoData[1] = vec4((iridescence * noise), 1.0);
	geoData[2] = finalNormal;
}


