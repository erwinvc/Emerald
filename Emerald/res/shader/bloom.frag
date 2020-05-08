#version 330
out vec3 outColor;

in vec2 fsUv;

uniform sampler2D _HDR;

void main()
{        
	vec3 color = texture(_HDR, fsUv).rgb;
	float brightness = dot(color, vec3(0.2126, 0.7152, 0.0722));
	if(brightness > 1.0) outColor = color;
}

