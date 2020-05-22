#version 330

struct Data {
	//vec3 normal;
	vec2 uv;
	mat3 TBNMatrix;
};

in Data fsData;

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

uniform sampler2D _Albedo;
uniform sampler2D _Normal;
uniform sampler2D _Metallic;
uniform sampler2D _Roughness;

out vec4 geoData[3];

uniform float roughness;
uniform float metallic;

//uniform sampler2DArray textureArray;

void main() {
	vec3 normalTex = (255.0/128.0 * (texture(_Normal, fsData.uv).rgb) - 1);
	vec3 normal = vec3(fsData.TBNMatrix * normalTex);
    geoData[0] = vec4(texture(_Roughness, fsData.uv).x, texture(_Metallic, fsData.uv).x, 0, 1);
	geoData[1] = vec4(texture(_Albedo, fsData.uv).rgb, 1.0);
	geoData[2] = vec4(normalize(_View * vec4(normal, 0.0)).xyz, 1.0);
}