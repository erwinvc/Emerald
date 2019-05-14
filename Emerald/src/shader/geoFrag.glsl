#version 330
in vec3 fsPos;
in vec3 fsNormal;
in vec2 fsUv;
in mat3 tbnMatrix;

uniform sampler2D _Albedo;
uniform sampler2D _Normal;
uniform float _NormalStrength;
uniform sampler2D _Specular;
uniform float _SpecularStrength;
uniform sampler2D _Emission;
uniform float _EmissionStrength;

out vec4 geoData[4];

void main(){
	vec4 diff = texture(_Albedo, vec2(1, -1) * fsUv).rgba;
	if (diff.a < 0.2) {
		discard;
	}

	vec3 normalMap = normalize(texture(_Normal, vec2(1, -1) * fsUv).rgb);
	normalMap = normalize((2*normalMap)-1);
	normalMap = normalize(tbnMatrix * normalMap);
	vec3 normal = mix(fsNormal, normalMap, _NormalStrength);

	vec3 specular = texture(_Specular, vec2(1, -1) * fsUv).rgb * _SpecularStrength;

	geoData[0] = vec4(specular.r, 0, 0, 0);
	geoData[1] = vec4(diff.rgb, 1);
	geoData[2] = vec4(normal, 1);
	geoData[3] = vec4(fsPos, 1);
}