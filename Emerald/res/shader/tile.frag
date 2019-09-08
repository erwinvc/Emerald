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
uniform vec4 _Boundaries;
out vec3 geoData[4];

uniform sampler2D texture_iridescence;
uniform sampler2D texture_noise;

uniform float scale1;
uniform float scale2;
uniform float scale3;

void main(){
	float normalStrength = fsStrengths.x;
	float specularStrength = fsStrengths.y;
	float emissionStrength = fsStrengths.z;

	vec4 diff = texture(_Albedo, vec3(fsUv, fsTextureID)).rgba;
	if (diff.a < 0.2) {
		discard;
	}
	
    vec3 BumpMapNormal = texture(_Normal, vec3(fsUv, fsTextureID)).xyz;
    BumpMapNormal = 2.0 * BumpMapNormal - 1;

    vec3 NewNormal = fsTBNMatrix * BumpMapNormal;
    NewNormal = normalize(NewNormal);

	vec3 finalNormal = mix(fsNormal, NewNormal, normalStrength);

	vec3 nview = normalize(fsViewDirection);
	vec3 nReflection = normalize(reflect(nview, finalNormal)); 
    float inverse_dot_view = 1.0 - max(dot(normalize(finalNormal), nview), 0.0);
    vec3 lookup_table_color = texture(texture_iridescence, vec2(inverse_dot_view, 0.0)).rgb;

	float specular = texture(_Specular, vec3(fsUv, fsTextureID)).a;

	float xDistance = (_Boundaries.y) - fsPos.y;
	float xDistance2 = _Boundaries.z - fsPos.x;

	//if(fsPos.x > 10 + noise(fsPos.xz)){
	geoData[0] = vec3(specular, 0, 0);
	geoData[1] = diff.rgb * mix(vec3(1), lookup_table_color, scale3);
	geoData[2] = finalNormal;
	geoData[3] = fsPos;
	//}
}