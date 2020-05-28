#version 330 core
out vec2 outColor;

void main() {
	float depth = gl_FragCoord.z;
	float dx = dFdx(depth);
	float dy = dFdy(depth);
	float moment2 = depth * depth + 0.25 * (dx * dx + dy * dy);
	outColor = vec2(depth, moment2);
}  