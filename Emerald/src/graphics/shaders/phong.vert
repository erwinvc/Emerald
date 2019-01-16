#version 460 core

in vec3 position;
in vec2 texCoords;
in vec3 normals;
in vec3 tangents;
in vec3 instancePosition;

uniform mat4 transform;
uniform mat4 projection;
uniform mat4 view;

uniform vec3 eyePosition;

out vec2 pass_texCoords;
out vec4 pass_worldPosition;
out vec3 pass_normals;
out mat3 pass_TBN;

void main(){
	pass_texCoords = texCoords;
	vec3 newPos = vec3(position.x + instancePosition.x, position.y + instancePosition.y, position.z + instancePosition.z);
	pass_worldPosition = transform * vec4(newPos, 1.0);
	pass_normals = (transform * vec4(normals, 0.0)).xyz;
	gl_Position = projection * view * pass_worldPosition;

	vec3 n = normalize(vec3(transform * vec4(normals, 0.0)));
	vec3 t = normalize(vec3(transform * vec4(tangents, 0.0)));
	t = normalize(t - dot(t, n) * n);

	vec3 bitangent = cross(t, n);
	pass_TBN = transpose(mat3(t, bitangent, n));
}