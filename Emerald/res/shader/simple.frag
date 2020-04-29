#version 330
out vec3 outColor[2];
  
in vec2 fsUv;
  
uniform sampler2D _SSR;
uniform float _BloomFactor;

void main() {
	vec3 col = texture(_SSR, fsUv).rgb;
	outColor[0] = col;
	outColor[1] = max(col - _BloomFactor, 0.0f);
}  