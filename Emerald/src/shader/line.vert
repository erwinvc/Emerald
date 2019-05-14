#version 330
layout(location = 0) in vec3 vsPos;
layout(location = 1) in vec3 vsLineBegin;
layout(location = 2) in vec3 vsLineEnd;

out vec3 fsPos;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;

void main(){
	vec3 lineOffset = gl_VertexID == 0 ? vsLineBegin : vsLineEnd;
    fsPos = vec3(vsPos.x + lineOffset.x, vsPos.y + lineOffset.y, vsPos.z + lineOffset.z);
    gl_Position = projectionMatrix * viewMatrix * vec4(fsPos, 1.0);
}