#version 330
in vec3 fsPos;
in vec3 fsNormal;
in vec2 fsUv;
in mat3 fsTBNMatrix;
in vec3 fsViewDirection;
in vec3 fsStrengths;
in float fsTextureID;

uniform sampler2DArray _Albedo;
uniform sampler2DArray _Normal;
uniform sampler2DArray _Specular;
uniform sampler2DArray _Emission;

uniform sampler2D _Iridescence;
uniform float _IridescenceStrength;

out vec3 geoData[4];

void main(){
	float normalStrength = fsStrengths.x;
	float specularStrength = fsStrengths.y;
	float emissionStrength = fsStrengths.z;

	vec4 albedo = texture(_Albedo, vec3(fsUv, fsTextureID)).rgba;
	if (albedo.a < 0.2) {
		discard;
	}
	
    vec3 textureNormal = (texture(_Normal, vec3(fsUv, fsTextureID)).xyz * 2.0f) - 1.0f;
    vec3 normal = normalize(fsTBNMatrix * textureNormal);
	vec3 finalNormal = mix(fsNormal, normal, normalStrength);

	vec3 nView = normalize(fsViewDirection);
	vec3 nReflection = normalize(reflect(nView, finalNormal)); 
    float inverseDotView = 1.0 - max(dot(normalize(finalNormal), nView), 0.0);
    vec3 iridescence = texture(_Iridescence, vec2(inverseDotView, 0.0)).rgb;

	float specular = texture(_Specular, vec3(fsUv, fsTextureID)).a;

	geoData[0] = vec3(specular, 0, 0);
	geoData[1] = albedo.rgb * mix(vec3(1), iridescence, _IridescenceStrength);
	geoData[2] = finalNormal;
	geoData[3] = fsPos;
}

