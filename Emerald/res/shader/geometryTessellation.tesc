#version 430 core

layout (vertices = 3) out;

struct Data {
	vec3 pos;
	vec3 normal;
	vec2 uv;
	mat3 TBNMatrix;
	vec3 viewDirection;
};

in Data csData[];
out Data esData[];

out float termIJ[];
out float termJK[];
out float termIK[];

uniform vec3 _CameraPosition;

uniform float _TessellationFactor = 4;
uniform float _TessellationSlope = 2;
uniform float _TessellationShift = 2;

float LodFactor(float dist){
	return max(0.0, _TessellationFactor/pow(dist, _TessellationSlope) + _TessellationShift);
}

float Phong(int i, vec3 q){
	vec3 q_minus_p = q - gl_in[i].gl_Position.xyz;
	return q[gl_InvocationID] - dot(q_minus_p, csData[i].normal)  * csData[i].normal[gl_InvocationID];
}

void main(void) {
	esData[gl_InvocationID].pos           = csData[gl_InvocationID].pos;
	esData[gl_InvocationID].normal        = csData[gl_InvocationID].normal;
	esData[gl_InvocationID].uv            = csData[gl_InvocationID].uv;
	esData[gl_InvocationID].TBNMatrix     = csData[gl_InvocationID].TBNMatrix;
	esData[gl_InvocationID].viewDirection = csData[gl_InvocationID].viewDirection;

	termIJ[gl_InvocationID] = Phong(0, gl_in[1].gl_Position.xyz) + Phong(1, gl_in[0].gl_Position.xyz);
	termJK[gl_InvocationID] = Phong(1, gl_in[2].gl_Position.xyz) + Phong(2, gl_in[1].gl_Position.xyz);
	termIK[gl_InvocationID] = Phong(2, gl_in[0].gl_Position.xyz) + Phong(0, gl_in[2].gl_Position.xyz);

	if (gl_InvocationID == 0) {
		vec3 faceMidPoint = vec3(gl_in[0].gl_Position + gl_in[1].gl_Position + gl_in[2].gl_Position) /3;

		float ds = distance(faceMidPoint, _CameraPosition);
		gl_TessLevelInner[0] = mix(1, gl_MaxTessGenLevel, LodFactor(ds));
		gl_TessLevelOuter[0] = mix(1, gl_MaxTessGenLevel, LodFactor(ds));
		gl_TessLevelOuter[1] = mix(1, gl_MaxTessGenLevel, LodFactor(ds));
		gl_TessLevelOuter[2] = mix(1, gl_MaxTessGenLevel, LodFactor(ds));
	}

	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}

