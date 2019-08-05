#version 400 core

in vec2 textureCoords;

out vec4 out_Color;

uniform sampler2D _Texture;
uniform vec4 _Color;

void main() {
	vec4 sampledColor = texture(_Texture, textureCoords);
	vec3 color = mix(sampledColor.xyz, _Color.xyz, _Color.a); 
    out_Color = vec4(color, sampledColor.a);
}
