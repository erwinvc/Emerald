#pragma once

class PhongShader : public Shader {
public:
    PhongShader() : Shader("Phong", "src/graphics/shaders/phong.vert", "src/graphics/shaders/phong.frag") {
        AddUniform("transformationMatrix");
        AddUniform("projectionMatrix");
        AddUniform("viewMatrix");
        AddUniform("ambient");
        //AddUniform("light.attenuation");
    }
};