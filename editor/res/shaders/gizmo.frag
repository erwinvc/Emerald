#version 330 core
in vec3 FragColor;
out vec4 FragOutput;

void main() {
    FragOutput = vec4(FragColor, 1.0);
}