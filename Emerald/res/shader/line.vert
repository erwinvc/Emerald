#version 330
//layout(location = 0) in vec3 vsPos;
layout(location = 0) in vec3 vsLineBegin;
layout(location = 1) in vec3 vsLineEnd;

out vec3 fsPos;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;

void main(){
    fsPos = gl_VertexID == 0 ? vsLineBegin : vsLineEnd;
    gl_Position = projectionMatrix * viewMatrix * vec4(fsPos, 1.0);
}