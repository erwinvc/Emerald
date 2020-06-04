#version 330

#include "includes/globalUniforms.incl"

in vec3 vsPos;
in vec3 vsIndex; //x = vertex; y = block side; z = texture;

out vec2 fsUv;
out vec3 fsNormal;
out vec3 fsTangent;

uniform vec3 _ChunkPos;

struct Data {
	vec3 uv;
	mat3 TBNMatrix;
};

out Data fsData;

vec2 texCoords[4] = vec2[4](
    vec2(0.0f, 0.0f),
    vec2(1.0f, 0.0f),
    vec2(1.0f, 1.0f),
    vec2(0.0f, 1.0f)
);

vec3 normals[6] = vec3[6](
	vec3( -1, 0, 0 ),
	vec3( 1, 0, 0 ),
	vec3( 0, 0, 1 ),
	vec3( 0, 0, -1 ),
	vec3( 0, 1, 0 ),
	vec3( 0, -1, 0 )
);

vec3 tangents[6] = vec3[6](
	vec3( 0, 0, -1 ),
	vec3( 0, 0, -1 ),
	vec3( 1, 0, 0 ),
	vec3( 1, 0, 0 ),
	vec3( 1, 0, 0 ),
	vec3( 1, 0, 0 )
);

vec2 GetTexCoords(float index){
	switch(int(index)){
		case 0:	return texCoords[0]; break;
		case 1:	return texCoords[1]; break;
		case 2:	return texCoords[2]; break;
		case 3:	return texCoords[3]; break;
	}
}

vec3 GetNormals(float index){
	switch(int(index)){
		case 0:	return normals[0]; break;
		case 1:	return normals[1]; break;
		case 2:	return normals[2]; break;
		case 3:	return normals[3]; break;
		case 4:	return normals[4]; break;
		case 5:	return normals[5]; break;
	}
}

vec3 GetTangents(float index){
	switch(int(index)){
		case 0:	return tangents[0]; break;
		case 1:	return tangents[1]; break;
		case 2:	return tangents[2]; break;
		case 3:	return tangents[3]; break;
		case 4:	return tangents[4]; break;
		case 5:	return tangents[5]; break;
	}
}

void main() {  
    vec3 pos = vsPos + _ChunkPos;
    gl_Position = _Projection * _View * vec4(pos, 1.0);
    vec3 n = GetNormals(vsIndex.y);
	vec3 t = GetTangents(vsIndex.y);
	vec3 b = cross(n, t);
	fsData.TBNMatrix = mat3(t, b, n);

    fsData.uv = vec3(GetTexCoords(vsIndex.x), vsIndex.z);
}