#version 330 core
out vec4 FragColor;
  
in vec2 fsUv;
  
uniform sampler2D _SSAO;

void main() {
	const int amount = 2;
    vec2 texelSize = 1.0f / vec2(textureSize(_SSAO, 0));
    float result = 0.0;
    for (int x = -amount; x < amount; ++x) 
    {
        for (int y = -amount; y < amount; ++y) 
        {
            vec2 offset = vec2(x, y) * texelSize;
            result += texture(_SSAO, fsUv + offset).r;
        }
    }
	float res = result / (2 * amount * 2 * amount);
    FragColor = vec4(res, res, res, 1);
}  