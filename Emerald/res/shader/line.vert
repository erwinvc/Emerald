#version 330
layout(location = 0) in vec3 vsLineBegin;
layout(location = 1) in vec3 vsLineEnd;
layout(location = 2) in vec4 vsColor;

out vec3 fsPos;
out vec4 fsColor;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;

void main(){
	fsColor = vsColor;
    fsPos = gl_VertexID == 0 ? vsLineBegin : vsLineEnd;
    gl_Position = projectionMatrix * viewMatrix * vec4(fsPos, 1.0);
}