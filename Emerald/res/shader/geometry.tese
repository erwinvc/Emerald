#version 430 core
layout (triangles, fractional_odd_spacing, ccw) in;

struct Data {
	vec3 pos;
	vec3 normal;
	vec2 uv;
	mat3 TBNMatrix;
	vec3 viewDirection;
};

in Data esData[];
out Data fsData;

uniform mat4 _ProjectionMatrix;
uniform mat4 _ViewMatrix;
uniform float _TessellationAlpha;

in float termIJ[];
in float termJK[];
in float termIK[];

vec2 interpolateVec2(vec2 v0, vec2 v1, vec2 v2){
    return vec2(gl_TessCoord.x) * v0 + vec2(gl_TessCoord.y) * v1 + vec2(gl_TessCoord.z) * v2;
}

vec3 interpolateVec3(vec3 v0, vec3 v1, vec3 v2){
    return vec3(gl_TessCoord.x) * v0 + vec3(gl_TessCoord.y) * v1 + vec3(gl_TessCoord.z) * v2;
} 

mat3 interpolateMat3(mat3 v0, mat3 v1, mat3 v2){
    return mat3(gl_TessCoord.x) * v0 + mat3(gl_TessCoord.y) * v1 + mat3(gl_TessCoord.z) * v2;
} 

void main(void){ 

	fsData.pos				= interpolateVec3(esData[0].pos, esData[1].pos, esData[2].pos);
	fsData.normal			= interpolateVec3(esData[0].normal, esData[1].normal, esData[2].normal);
	fsData.uv				= interpolateVec2(esData[0].uv, esData[1].uv, esData[2].uv);
	fsData.TBNMatrix		= interpolateMat3(esData[0].TBNMatrix, esData[1].TBNMatrix, esData[2].TBNMatrix);
	fsData.viewDirection	= interpolateVec3(esData[0].viewDirection, esData[1].viewDirection, esData[2].viewDirection);

	vec3 barPos = gl_TessCoord[0]*gl_in[0].gl_Position.xyz + gl_TessCoord[1]*gl_in[1].gl_Position.xyz + gl_TessCoord[2]*gl_in[2].gl_Position.xyz;
	vec3 termIJ = vec3(termIJ[0], termIJ[1], termIJ[2]);
	vec3 termJK = vec3(termJK[0], termJK[1], termJK[2]);
	vec3 termIK = vec3(termIK[0], termIK[1], termIK[2]);

	vec3 tc1 = gl_TessCoord;
	vec3 tc2 = tc1*tc1;
	vec3 phongPos   = tc2[0]*gl_in[0].gl_Position.xyz
	                + tc2[1]*gl_in[1].gl_Position.xyz
	                + tc2[2]*gl_in[2].gl_Position.xyz
	                + tc1[0]*tc1[1]*termIJ
	                + tc1[1]*tc1[2]*termJK
	                + tc1[2]*tc1[0]*termIK;

					vec3 finalPos = (1.0 -_TessellationAlpha) * barPos + _TessellationAlpha * phongPos;

	gl_Position  = _ProjectionMatrix * _ViewMatrix * vec4(finalPos,1.0);
}