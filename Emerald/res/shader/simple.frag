#version 330 core
out vec4 outColor;
  
in vec2 fsUv;
  
uniform sampler2D _SSR;

void main() {
	outColor = texture(_SSR, fsUv);
}  