#pragma once

class BasicShader : public Shader {
public:
	BasicShader() : Shader("Basic", "src/graphics/shaders/basic.vert", "src/graphics/shaders/basic.frag")
	{
		AddUniform("transformationMatrix");
		AddUniform("projectionMatrix");
		AddUniform("viewMatrix");

        AddUniform("ambientColor");
        AddUniform("ambientIntensity");

		AddUniform("eyePos");
        AddUniform("color");
		AddUniform("intensity");
		AddUniform("direction");
        AddUniform("tex");
        AddUniform("texNormal");
        AddUniform("texHeight");
        AddUniform("heightOffset");

        AddUniform("specularIntensity");
        AddUniform("specularPower");
        
        for (int i = 0; i < 4; i++) {
            AddUniform(va("%s%d%s", "lightPosition[", i, "]").c_str());
            AddUniform(va("%s%d%s", "lightColor[", i, "]").c_str());
            //AddUniform(va("%s%d%s", "attenuation[", i, "]").c_str());
        }
	}
};