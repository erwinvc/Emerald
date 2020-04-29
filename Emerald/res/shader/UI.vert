#version 330

layout(location = 0) in vec3 vsPosition;
layout(location = 1) in vec2 vsUv;
layout(location = 2) in float vsTextureID;
layout(location = 3) in vec4 vsColor;
layout(location = 4) in vec3 vsAtlasValues;

struct Data {
	vec2 uv;
	float textureID;
	vec4 color;
};

out Data fsData;

uniform mat4 _Projection;
uniform mat4 _View;

void main(){
	fsData.textureID = vsTextureID;
	fsData.color = vsColor;

	gl_Position = _Projection * _View * vec4(vsPosition, 1.0);
	fsData.uv = (vsUv/vsAtlasValues.x) + vsAtlasValues.yz;
	//gl_Position = vec4(vsPosition, 1.0);
}