#version 330 core
layout (location = 0) in vec3 vsPos;
layout (location = 1) in vec2 vsUv;

out vec2 fsUv;

void main() {
	fsUv = vsUv;
	gl_Position = vec4(vsPos, 1.0);
}