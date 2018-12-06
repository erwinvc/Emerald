#version 400 core

in vec4 position;
in float offsetX;
in float offsetY;
in float offsetZ;
in vec4 color;

uniform mat4 projection;
out vec4 pass_color;

void main(){
	gl_Position = vec4(position.x + offsetX, position.y + offsetY, position.z + offsetZ, position.w) * projection;
	pass_color = color;
}