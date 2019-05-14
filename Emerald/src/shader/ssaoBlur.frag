#version 330 core
out vec4 FragColor;
  
in vec2 fsUv;
  
uniform sampler2D _SSAO;

void main() {
    vec2 texelSize = 1.0 / vec2(textureSize(_SSAO, 0));
    float result = 0.0;
    for (int x = -2; x < 2; ++x) 
    {
        for (int y = -2; y < 2; ++y) 
        {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            result += texture(_SSAO, fsUv + offset).r;
        }
    }
	float res = result / (4.0 * 4.0);
    FragColor = vec4(res, res, res, 1);
}  