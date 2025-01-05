#version 430 core
out vec2 outMoments;

void main() {
    float depth = gl_FragCoord.z;

    float dx = dFdx(depth);
    float dy = dFdy(depth);

    float moment2 = depth * depth + 0.25 * (dx*dx + dy*dy);

    outMoments = vec2(depth, moment2);
}