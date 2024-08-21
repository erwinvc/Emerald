#version 430 core

struct Data {
	vec3 pos;
	vec2 uv;
};

in Data vsData;
out vec4 FragColor;

uniform int colorIndex;
uniform vec4 color[2];

void main(){
    FragColor = color[colorIndex]; // White color
}


