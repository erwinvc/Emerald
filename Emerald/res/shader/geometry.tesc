#version 430 core

layout (vertices = 3) out;

in vec3 csPos[];
in vec3 csNormal[];
in vec2 csUv[];
in mat3 csTBNMatrix[];
in vec3 cstangent[];
in vec3 csViewDirection[];

out vec3 esPos[];
out vec3 esNormal[];
out vec2 esUv[];
out mat3 esTBNMatrix[];
out vec3 estangent[];
out vec3 esViewDirection[];

uniform vec3 cameraPosition;

uniform float t1 = 4;
uniform float t2 = 2;
uniform float t3 = 2;
uniform float t4 = 2;

float LodFactor(float dist){
	float tl = max(0.0, t1/pow(dist, t2) + t3);
	return tl;
}

void main(void) {
	esPos[gl_InvocationID] = csPos[gl_InvocationID];
	esNormal[gl_InvocationID] = csNormal[gl_InvocationID];
	esUv[gl_InvocationID] = csUv[gl_InvocationID];
	esTBNMatrix[gl_InvocationID] = csTBNMatrix[gl_InvocationID];
	estangent[gl_InvocationID] = cstangent[gl_InvocationID];
	esViewDirection[gl_InvocationID] = csViewDirection[gl_InvocationID];

	if (gl_InvocationID == 0) {

		vec3 abMid = vec3(gl_in[0].gl_Position + gl_in[1].gl_Position + gl_in[2].gl_Position) /3;

		float ds = distance(abMid, cameraPosition);
		gl_TessLevelInner[0] = mix(1, gl_MaxTessGenLevel, LodFactor(ds));
		gl_TessLevelOuter[0] = mix(1, gl_MaxTessGenLevel, LodFactor(ds));
		gl_TessLevelOuter[1] = mix(1, gl_MaxTessGenLevel, LodFactor(ds));
		gl_TessLevelOuter[2] = mix(1, gl_MaxTessGenLevel, LodFactor(ds));
	}

	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}