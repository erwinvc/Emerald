#version 400 core

in vec3 position;
in vec2 textureCoords;
in vec3 normal;
in vec3 tangent;
in vec3 offset;

uniform mat4 transformationMatrix;
uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform vec3 eyePos;

out vec2 pass_textureCoords;
out vec4 pass_worldPosition;
out vec3 pass_normal;
out mat3 tbnMatrix;

void main(){
	pass_textureCoords = textureCoords;
	//int x = gl_InstanceID / 100;
	//int y = gl_InstanceID % 100;
	vec3 newPos = vec3(position.x + offset.x, position.y + offset.y, position.z + offset.z);
	pass_worldPosition = transformationMatrix * vec4(newPos, 1.0);
	pass_normal = (transformationMatrix * vec4(normal, 0.0)).xyz;
	gl_Position = projectionMatrix * viewMatrix * pass_worldPosition;

	vec3 n = normalize(vec3(transformationMatrix * vec4(normal, 0.0)));
	vec3 t = normalize(vec3(transformationMatrix * vec4(tangent, 0.0)));
	t = normalize(t - dot(t, n) * n);

	vec3 bitangent = cross(t, n);
	tbnMatrix = mat3(t, bitangent, n);
	tbnMatrix = transpose(tbnMatrix);

	//pass_color = vec4(position.x + 0.5f, 0.0f, position.y + 0.5f, 1.0f);
}