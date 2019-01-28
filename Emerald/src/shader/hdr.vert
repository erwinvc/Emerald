#version 400 core

in vec2 position;

out vec2 textureCoords;

void main() {
    gl_Position = vec4(position, 0, 1);
    textureCoords = vec2((position.x+1.0)/2.0, (position.y+1.0)/2.0);
    //textureCoords += texPos;
    //textureCoords /= texResolution;
    //textureCoords *= texSize;
}