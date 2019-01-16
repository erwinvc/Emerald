#version 330

layout(location = 0) in vec3 vsPos;
out vec4 fsPos;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;

uniform float uLightRadius;
uniform vec3 uLightPosition;

void main()
{
  vec4 pos = projectionMatrix * viewMatrix * vec4((vsPos * uLightRadius) + uLightPosition, 1.0);

  gl_Position = pos;
  fsPos = pos;
}