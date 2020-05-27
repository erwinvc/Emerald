#version 330

in vec3 vsPosition;
in vec2 vsUv;
in float vsTextureID;
in vec4 vsColor;
in vec3 vsAtlasValues;

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