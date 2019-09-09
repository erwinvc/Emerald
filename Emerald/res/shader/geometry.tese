#version 430 core
layout (triangles, fractional_odd_spacing, ccw) in;

in vec3 esPos[];
in vec3 esNormal[];
in vec2 esUv[];
in mat3 esTBNMatrix[];
in vec3 estangent[];
in vec3 esViewDirection[];

out vec3 fsPos;
out vec3 fsNormal;
out vec2 fsUv;
out mat3 fsTBNMatrix;
out vec3 fstangent;
out vec3 fsViewDirection;

uniform mat4 transformationMatrix;
uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;

vec2 interpolate2D(vec2 v0, vec2 v1, vec2 v2)
{
    return vec2(gl_TessCoord.x) * v0 + vec2(gl_TessCoord.y) * v1 + vec2(gl_TessCoord.z) * v2;
}

vec3 interpolate3D(vec3 v0, vec3 v1, vec3 v2)
{
    return vec3(gl_TessCoord.x) * v0 + vec3(gl_TessCoord.y) * v1 + vec3(gl_TessCoord.z) * v2;
} 

mat3 interpolateMat3(mat3 v0, mat3 v1, mat3 v2)
{
    return mat3(gl_TessCoord.x) * v0 + mat3(gl_TessCoord.y) * v1 + mat3(gl_TessCoord.z) * v2;
} 

void main(void){ 

	fsPos = interpolate3D(esPos[0], esPos[1], esPos[2]);
	fsNormal = interpolate3D(esNormal[0], esNormal[1], esNormal[2]);
	fsUv = interpolate2D(esUv[0], esUv[1], esUv[2]);
	fsTBNMatrix = interpolateMat3(esTBNMatrix[0], esTBNMatrix[1], esTBNMatrix[2]);
	fstangent = interpolate3D(estangent[0], estangent[1], estangent[2]);
	fsViewDirection = interpolate3D(esViewDirection[0], esViewDirection[1], esViewDirection[2]);

	gl_Position= projectionMatrix * viewMatrix *transformationMatrix * (gl_TessCoord.x*gl_in[0].gl_Position+gl_TessCoord.y*gl_in[1].gl_Position+gl_TessCoord.z*gl_in[2].gl_Position);
}