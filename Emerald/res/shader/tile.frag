#version 330

struct Data {
	vec3 pos;
	vec3 normal;
	vec2 uv;
	mat3 TBNMatrix;
	float textureID;
};

in Data fsData;

uniform sampler2DArray _Albedo;
uniform sampler2DArray _Normal;
uniform sampler2DArray _Roughness;
uniform sampler2DArray _Metallic;
uniform sampler2DArray _Emission;

out vec4 geoData[4];

void main(){
	vec3 newUV = vec3(fsData.uv, 0);

	vec4 albedo = texture(_Albedo, newUV).rgba;
	if (albedo.a < 0.2) discard;

	float roughness = texture(_Roughness, newUV).r;
	float metallic = texture(_Metallic, newUV).r;
	float emission = texture(_Emission, newUV).r;
	vec3 tangentNormal = (texture(_Normal, newUV).xyz * 2.0f) - 1.0f;
    vec3 worldNormal = normalize(fsData.TBNMatrix * tangentNormal);

	geoData[0] = vec4(roughness, metallic, emission, 0);
	geoData[1] = vec4(albedo.rgb, 1.0);
	geoData[2] = vec4(worldNormal, 1.0);
	geoData[3] = vec4(fsData.pos, 1.0);
}