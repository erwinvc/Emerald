#version 330

#include "includes/globalUniforms.incl"
#include "includes/packing.incl"

struct Data {
	vec3 uv;
	mat3 TBNMatrix;
};

in Data fsData;

uniform sampler2DArray _Albedo;
uniform sampler2DArray _Normal;
uniform sampler2DArray _Metallic;
uniform sampler2DArray _Roughness;
uniform sampler2DArray _Emission;

out vec4 geoData[3];

uniform float _Time;
//uniform sampler2DArray textureArray;

vec3 ApplyHue(vec3 col, float hueAdjust) {
    const vec3 k = vec3(0.57735, 0.57735, 0.57735);
    float cosAngle = cos(hueAdjust);
    return col * cosAngle + cross(k, col) * sin(hueAdjust) + k * dot(k, col) * (1.0 - cosAngle);
}

void main() {
	vec3 normalTex = (255.0/128.0 * (texture(_Normal, fsData.uv).rgb) - 1);
	vec3 normal = vec3(fsData.TBNMatrix * normalTex);
	vec3 emission = texture(_Emission, fsData.uv).xyz;
	//emission = ApplyHue(emission, _Time / 1000);
    geoData[0] = vec4(texture(_Roughness, fsData.uv).x, texture(_Metallic, fsData.uv).x * 0.8, emission.x, emission.y);
	geoData[1] = vec4(texture(_Albedo, fsData.uv).rgb, emission.z);
	geoData[2] = vec4(normalize(_View * vec4(normal, 0.0)).xyz, 0);
}