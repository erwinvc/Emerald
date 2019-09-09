#version 330
in vec3 fsPos;
in vec3 fsNormal;
in vec2 fsUv;
in mat3 fsTBNMatrix;
in vec3 fstangent;
in vec3 fsViewDirection;

uniform sampler2D _Albedo;
uniform sampler2D _Normal;
uniform float _NormalStrength;
uniform sampler2D _Specular;
uniform float _SpecularStrength;
uniform sampler2D _Emission;
uniform float _EmissionStrength;

out vec3 geoData[4];

uniform sampler2D texture_iridescence;
uniform sampler2D texture_noise;

uniform float scale1;
uniform float scale2;
uniform float scale3;

void main(){
	vec4 diff = texture(_Albedo, fsUv).rgba;
	if (diff.a < 0.2) {
		discard;
	}
	
    vec3 BumpMapNormal = texture(_Normal, fsUv).xyz;
    BumpMapNormal = 2.0 * BumpMapNormal - vec3(1.0, 1.0, 1.0);
    vec3 NewNormal;

    NewNormal = fsTBNMatrix * BumpMapNormal;
    NewNormal = normalize(NewNormal);

	vec3 normal = mix(fsNormal, NewNormal, _NormalStrength);

	vec3 nview = normalize(fsViewDirection);
	vec3 n_reflection = normalize(reflect(nview, normal)); 
	vec3 noise_vector = BumpMapNormal * scale1;
	noise_vector =  mix(noise_vector, (texture(texture_noise, fsUv).xyz - vec3(0.5)) * 0.5f, scale2);
    float inverse_dot_view = 1.0 - max(dot(normalize(normal + noise_vector), nview), 0.0);
    vec3 lookup_table_color = texture(texture_iridescence, vec2(inverse_dot_view, 0.0)).rgb;

	float specular = texture(_Specular, fsUv).a;

	geoData[0] = vec3(specular, 0, 0);
	geoData[1] = diff.rgb * mix(vec3(1), lookup_table_color, scale3);
	geoData[2] = normal;
	geoData[3] = fsPos;
}