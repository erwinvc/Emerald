#version 330 core

#include "includes/globalUniforms.incl"

in vec3 vsPos;
in vec2 vsIndex; //x = vertex; y = block side;

uniform vec3 _ChunkPos;

void main()
{
    vec3 pos = vsPos + _ChunkPos;
    gl_Position = vec4(pos, 1.0);
}  