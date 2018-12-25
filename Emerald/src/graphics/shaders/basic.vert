#version 400 core

in vec3 position;
in vec2 textureCoords;
in vec3 normal;

uniform mat4 transformationMatrix;
uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;

out vec2 pass_textureCoords;
out vec4 pass_worldPosition;
out vec3 pass_normal;
void main(){
	pass_textureCoords = textureCoords;
	pass_normal = (transformationMatrix * vec4(normal, 0.0)).xyz;
	pass_worldPosition = transformationMatrix * vec4(position, 1.0);
	gl_Position = projectionMatrix * viewMatrix * pass_worldPosition;
	//pass_color = vec4(position.x + 0.5f, 0.0f, position.y + 0.5f, 1.0f);
}