#version 400 core

in vec2 position;
in vec2 uv;

out vec2 textureCoords;

uniform mat4 projectionMatrix;
uniform mat4 transformationMatrix;

void main() {
    gl_Position = projectionMatrix * transformationMatrix * vec4(position, 0.0, 1.0);
    textureCoords = uv;
}

