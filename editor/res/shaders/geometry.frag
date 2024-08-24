#version 430 core

struct Data {
	vec4 pos;
	vec2 uv;
};

in Data vsData;
out vec4 FragColor;

uniform int colorIndex;
uniform vec4 color[2];
uniform sampler2D tex;

void main(){
	FragColor = texture(tex, vsData.uv) * color[colorIndex];
}