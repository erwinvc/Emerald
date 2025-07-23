@program id

@vs
#version 460 core

layout (location = 0) in vec3 aPos;

uniform mat4 _ModelMatrix;
uniform mat4 _ViewMatrix;
uniform mat4 _ProjectionMatrix;

void main() {
    gl_Position = _ProjectionMatrix * _ViewMatrix * _ModelMatrix * vec4(aPos,1.0);
}
@end

@fs
#version 460 core
layout(location = 0) out uint o_ID;
layout(location = 1) out vec4 o_Debug;

uniform uint _ID;
void main() {
	o_ID = _ID;

    uint  h = _ID * 1664525u + 1013904223u;   // LCG hash
    h     ^= (h >> 16);
    h     ^= (h >>  8);
    
    vec3  rgb = vec3( (h & 255u) / 255.0,
                      ((h >> 8) & 255u) / 255.0,
                      ((h >>16) & 255u) / 255.0 );
    
    o_Debug = vec4(rgb, 1.0);
}
@end