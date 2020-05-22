#version 330

in vec3 vsPos;
in vec2 vsIndex; //x = vertex; y = block side;

out vec2 fsUv;
out vec3 fsNormal;
out vec3 fsTangent;

uniform vec3 _ChunkPos;

struct Data {
	//vec3 normal;
	vec2 uv;
	mat3 TBNMatrix;
};

out Data fsData;

layout (std140) uniform GlobalUniforms {
	vec3 _CameraPosition;
	mat4 _Projection;
	mat4 _View;
	mat4 _InverseProjection;
	mat4 _InverseView;
    bool _SSAOEnabled;
	vec2 _CameraPlanes;
	vec2 _ViewPort;
};

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

void main() {  
    vec3 pos = vsPos + _ChunkPos;
    gl_Position = _Projection * _View * vec4(pos, 1.0);
    vec3 n = normals[int(vsIndex.y)];
    vec3 t = tangents[int(vsIndex.y)];
	vec3 b = cross(n, t);
	fsData.TBNMatrix = mat3(t, b, n);

    fsData.uv = texCoords[int(vsIndex.x)];
}