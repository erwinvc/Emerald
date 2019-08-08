#version 330
layout(location = 0) in vec3 vsPos;
layout(location = 1) in vec3 vsNormal;
layout(location = 2) in vec2 vsUv;
layout(location = 3) in vec3 vsTangents;
layout(location = 4) in vec3 vsBitangents;
layout(location = 5) in vec2 vsPosition;
layout(location = 6) in float vsTransformIndex;

out vec3 fsPos;
out vec3 fsNormal;
out vec2 fsUv;
out mat3 fsTBNMatrix;
out vec3 fstangent;
out vec3 fsViewDirection;

uniform mat4 _ProjectionMatrix;
uniform mat4 _ViewMatrix;

uniform mat4 _Transforms[6];

void main(){
	mat3 transformRotation = mat3(_Transforms[int(vsTransformIndex)]);

	vec3 worldPos = (_Transforms[int(vsTransformIndex)] * vec4(vsPos, 1.0)).xyz;
	vec3 worldNormal = (transformRotation * vsNormal).xyz;
	vec3 worldTangent = (transformRotation * vsTangents).xyz;
	vec3 worldBiTangent = (transformRotation * vsBitangents).xyz;

	vec3 tangent = normalize(worldTangent - dot(worldNormal, worldTangent) * worldNormal);
	if (dot(cross(worldNormal, tangent), worldBiTangent) < 0.0f) tangent *= -1.0f;
    fsTBNMatrix = mat3(tangent, worldBiTangent, worldNormal);

	fsPos = vec3(worldPos.x + vsPosition.x + 0.5f, worldPos.y, worldPos.z + vsPosition.y + 0.5f);

	fsNormal = worldNormal;
	fsUv = vsUv;

	fsViewDirection = (inverse(_ViewMatrix) * vec4(0.0, 0.0, 0.0, 1.0)).xyz - fsPos;
	gl_Position = _ProjectionMatrix * _ViewMatrix * vec4(fsPos, 1.0);
}