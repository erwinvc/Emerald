#version 330
layout(location = 0) in vec3 vsPos;
layout(location = 1) in vec3 vsNormal;
layout(location = 2) in vec2 vsUv;
layout(location = 3) in vec3 vsTangents;
layout(location = 4) in vec3 vsBitangents;
layout(location = 5) in vec2 vsPosition;
layout(location = 6) in float vsTransformIndex;
//layout(location = 7) in vec4 vsHeights;

out vec3 fsPos;
out vec2 fsUv;
out vec3 fsNormal;
out mat3 fsTBNMatrix;
out vec3 fstangent;
out vec3 fsViewDirection;

uniform mat4 _ProjectionMatrix;
uniform mat4 _ViewMatrix;

uniform mat4 _Transforms[6];

void main(){
	float yDot = abs(dot(vsNormal, vec3(0, 1, 0)));


	mat4 selectedMatrix = _Transforms[int(vsTransformIndex)];

	if(yDot > 0.9f){
		vec3 tangent = normalize(vsTangents - dot(vsNormal, vsTangents) * vsNormal);
		fsTBNMatrix = mat3(tangent, vsBitangents, vsNormal);
		fsNormal = vsNormal;
	} else{
		vec3 worldNormal = (selectedMatrix * vec4(vsNormal, 0.0)).xyz;
		vec3 worldTangent = (selectedMatrix * vec4(vsTangents, 0.0)).xyz;
		vec3 worldBiTangent = (selectedMatrix * vec4(vsBitangents, 0.0)).xyz;
		vec3 tangent = normalize(worldTangent - dot(worldNormal, worldTangent) * worldNormal);
		fsTBNMatrix = mat3(tangent, worldBiTangent, worldNormal);
		fsNormal = worldNormal;
	}

	vec3 worldPos = (selectedMatrix * vec4(vsPos, 1.0)).xyz;
	fsPos = vec3(worldPos.x + vsPosition.x + 0.5f, worldPos.y, worldPos.z + vsPosition.y + 0.5f);


	fsUv = vsUv;
	if(yDot > 0.9f){
		fsUv = fsPos.zx;
	}

	fsViewDirection = (inverse(_ViewMatrix) * vec4(0.0, 0.0, 0.0, 1.0)).xyz - fsPos;
	gl_Position = _ProjectionMatrix * _ViewMatrix * vec4(fsPos, 1.0);
}