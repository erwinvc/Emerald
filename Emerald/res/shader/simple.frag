#version 330

#include "includes/globalUniforms.incl"

out vec3 outColor;
  
in vec2 fsUv;
  
uniform sampler2D _SSR;

void main() {
	vec3 col = texture(_SSR, fsUv).rgb;
	outColor = col;
}  