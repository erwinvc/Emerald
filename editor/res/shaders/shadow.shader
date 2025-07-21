@program Shadow

@vs
#version 430 core

layout(location = 0) in vec3 aPosition;

uniform mat4 _LightSpaceMatrix;
uniform mat4 _ModelMatrix;

void main()
{
    gl_Position = _LightSpaceMatrix * _ModelMatrix * vec4(aPosition, 1.0);
}
@end

@fs
#version 430 core
out vec2 outMoments;

void main() {
    float depth = gl_FragCoord.z;

    float dx = dFdx(depth);
    float dy = dFdy(depth);

    float moment2 = depth * depth + 0.25 * (dx*dx + dy*dy);

    outMoments = vec2(depth, moment2);
}
@end