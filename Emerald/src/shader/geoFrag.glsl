#version 330
in vec3 fsPos;
in vec3 fsNormal;
in vec2 fsUv;
in mat3 tbnMatrix;

uniform sampler2D uDiffTex;
uniform sampler2D uBumpTex;
uniform int hasBump;

out vec4 geoData[3];

void main(){
	vec4 diff = texture(uDiffTex, vec2(1, -1) * fsUv).rgba;
	if (diff.a < 0.2) {
		discard;
	}


	vec3 normal;
	if(hasBump == 1){
		normal = normalize(texture(uBumpTex, vec2(1, -1) * fsUv).rgb);
		normal = normalize((2*normal)-1);
		normal = normalize(tbnMatrix * normal);
	} else {
		normal = fsNormal;
	}

	geoData[0] = vec4(diff.rgb, 1);
	geoData[1] = vec4(normal, 1);
	geoData[2] = vec4(fsPos, 1);
}