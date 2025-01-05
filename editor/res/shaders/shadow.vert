#version 430 core

layout(location = 0) in vec3 aPosition;

uniform mat4 _LightSpaceMatrix;
uniform mat4 _ModelMatrix;

void main()
{
    gl_Position = _LightSpaceMatrix * _ModelMatrix * vec4(aPosition, 1.0);
}