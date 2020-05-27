#version 330

#include "includes/globalUniforms.incl"

in vec3 vsPos;
in vec4 vsColor;

out vec4 fsColor;


void main(){
	fsColor = vsColor;
    gl_Position = _Projection * _View * vec4(vsPos, 1.0);
}