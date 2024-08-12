#version 430 core

struct Data {
	vec3 pos;
	vec2 uv;
};

in Data vsData;
out vec4 FragColor;

void main(){
    FragColor = vec4(1.0, 0.0, 0.0, 1.0); // White color
}


