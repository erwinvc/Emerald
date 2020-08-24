#version 330

#include "includes/globalUniforms.incl"

in uvec2 vsData;
//in ivec3 vsPos;
//in vec3 vsIndex; //x = vertex; y = block side; z = texture;

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
    vec2(0.0, 0.0),
    vec2(1.0, 0.0),
    vec2(1.0, 1.0),
    vec2(0.0, 1.0)
);

vec3 normals[6] = vec3[6](
	vec3(0, -1, 0),
	vec3(0, 1, 0),
	vec3(0, 0, -1),
	vec3(0, 0, 1),
	vec3(-1, 0, 0),
	vec3(1, 0, 0)
);

	//vec3(-1, 0, 0),
	//vec3(1, 0, 0),
	//vec3(0, 0, 1),
	//vec3(0, 0, -1),
	//vec3(0, 1, 0),
	//vec3(0, -1, 0)


vec3 tangents[6] = vec3[6](
	vec3(-1, 0, 0),
	vec3(1, 0, 0),
	vec3(-1, 0, 0),
	vec3(1, 0, 0),
	vec3(0, 0, -1),
	vec3(0, 0, 1)
);

	//vec3(0, 0, -1), L
	//vec3(0, 0, -1), R
	//vec3(1, 0, 0), F
	//vec3(1, 0, 0), B
	//vec3(1, 0, 0), T
	//vec3(1, 0, 0) B

vec2 GetTexCoords(float index){
	switch(int(index)){
		case 0:	return texCoords[0]; break;
		case 1:	return texCoords[1]; break;
		case 2:	return texCoords[2]; break;
		case 3:	return texCoords[3]; break;
	}
}

vec3 GetNormals(int index){
	switch(int(index)){
		case 0:	return normals[0]; break;
		case 1:	return normals[1]; break;
		case 2:	return normals[2]; break;
		case 3:	return normals[3]; break;
		case 4:	return normals[4]; break;
		case 5:	return normals[5]; break;
	}
}

vec3 GetTangents(int index){
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

	float x = float(vsData.x & 0x3FFu);
	float y = float((vsData.x & 0xFFC00u) >> 10u);
	float z = float((vsData.x & 0x3FF00000u) >> 20u);
	vec3 pos = vec3(x, y, z);
	pos *= 0.0625;
	pos += _ChunkPos;

	float index = float((vsData.x & 0xC0000000u) >> 30u);

	float material = float((vsData.y & 0xFFFu));
	//uint side = uint((vsData.y & 0x7000u) >> 12u);

	int side = int((vsData.y & 0x7000u) >> 12u);

	float uvX = float((vsData.y & 0xF8000u) >> 15u) * 0.0625;
	float uvY = float((vsData.y & 0x1F00000u) >> 20u) * 0.0625;

    gl_Position = _Projection * _View * vec4(pos, 1.0);
    vec3 n = GetNormals(side);
	vec3 t = GetTangents(side);
	vec3 b = cross(n, t);
	fsData.TBNMatrix = mat3(t, b, n);

    fsData.uv = vec3(uvX, uvY, material);
}