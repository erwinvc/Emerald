#version 330

struct Data {
	vec2 uv;
	float textureID;
	vec4 color;
};

in Data fsData;

uniform sampler2D _Textures[32];

out vec4 outColor;
void main(){
	vec4 color = texture(_Textures[int(fsData.textureID)], fsData.uv);
	if(color.a < 0.01) discard;
	outColor = vec4(color.rgb * fsData.color.rgb, fsData.color.a * color.a);
	//outColor = vec4(1.0, 1.0, 1.0, 1.0);
}