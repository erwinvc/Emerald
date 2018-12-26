#version 400 core

in vec3 position;
in vec2 textureCoords;
in vec3 normal;
in vec3 tangent;

uniform mat4 transformationMatrix;
uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;

out vec2 pass_textureCoords;
out vec4 pass_worldPosition;
out mat3 tbnMatrix;

void main(){
	pass_textureCoords = textureCoords;
	pass_worldPosition = transformationMatrix * vec4(position, 1.0);
	gl_Position = projectionMatrix * viewMatrix * pass_worldPosition;

	vec3 n = normalize((transformationMatrix * vec4(normal, 0.0)).xyz);
	vec3 t = normalize((transformationMatrix * vec4(tangent, 0.0)).xyz);
	t = normalize(t - dot(t, n) * n);

	vec3 bitangent = cross(t, n);

	tbnMatrix = mat3(t, bitangent, n);

	//pass_color = vec4(position.x + 0.5f, 0.0f, position.y + 0.5f, 1.0f);
}