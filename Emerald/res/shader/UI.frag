#version 330

struct Data {
	vec2 uv;
	float textureID;
	vec4 color;
};

in Data fsData;

uniform sampler2D _Textures[32];

vec4 GetTextureColor(float index){
	switch(int(index)){
		case 0: return texture(_Textures[0], fsData.uv); break;
		case 1: return texture(_Textures[1], fsData.uv); break;
		case 2: return texture(_Textures[2], fsData.uv); break;
		case 3: return texture(_Textures[3], fsData.uv); break;
		case 4: return texture(_Textures[4], fsData.uv); break;
		case 5: return texture(_Textures[5], fsData.uv); break;
		case 6: return texture(_Textures[6], fsData.uv); break;
		case 7: return texture(_Textures[7], fsData.uv); break;
		case 8: return texture(_Textures[8], fsData.uv); break;
		case 9: return texture(_Textures[9], fsData.uv); break;
		case 10: return texture(_Textures[10], fsData.uv); break;
		case 11: return texture(_Textures[11], fsData.uv); break;
		case 12: return texture(_Textures[12], fsData.uv); break;
		case 13: return texture(_Textures[13], fsData.uv); break;
		case 14: return texture(_Textures[14], fsData.uv); break;
		case 15: return texture(_Textures[15], fsData.uv); break;
		case 16: return texture(_Textures[16], fsData.uv); break;
		case 17: return texture(_Textures[17], fsData.uv); break;
		case 18: return texture(_Textures[18], fsData.uv); break;
		case 19: return texture(_Textures[19], fsData.uv); break;
		case 20: return texture(_Textures[20], fsData.uv); break;
		case 21: return texture(_Textures[21], fsData.uv); break;
		case 22: return texture(_Textures[22], fsData.uv); break;
		case 23: return texture(_Textures[23], fsData.uv); break;
		case 24: return texture(_Textures[24], fsData.uv); break;
		case 25: return texture(_Textures[25], fsData.uv); break;
		case 26: return texture(_Textures[26], fsData.uv); break;
		case 27: return texture(_Textures[27], fsData.uv); break;
		case 28: return texture(_Textures[28], fsData.uv); break;
		case 29: return texture(_Textures[29], fsData.uv); break;
		case 30: return texture(_Textures[30], fsData.uv); break;
		case 31: return texture(_Textures[31], fsData.uv); break;
	}
}

out vec4 outColor;
void main(){
	vec4 color = GetTextureColor(fsData.textureID);
	if(color.a < 0.01) discard;
	outColor = vec4(color.rgb * fsData.color.rgb, fsData.color.a * color.a);
	//outColor = vec4(1.0, 1.0, 1.0, 1.0);
}