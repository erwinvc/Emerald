#version 330

#include "includes/globalUniforms.incl"

in vec3 vsPos;
in vec3 vsNormal;
in vec2 vsUv;
in vec3 vsTangent;
in vec3 vsBitangent;

struct Data {
	vec3 fsViewPos;
	vec3 fsWorldPos;
};

out Data fsData;

uniform mat4 _TransformationMatrix;

void main(){
	mat4 MVMatrix = _View * _TransformationMatrix;

	fsData.fsViewPos = (MVMatrix * vec4(vsPos, 1.0)).xyz;
	fsData.fsWorldPos = (_TransformationMatrix * vec4(vsPos, 1.0)).xyz;

	gl_Position = _Projection * _View * _TransformationMatrix * vec4(vsPos, 1.0);
}