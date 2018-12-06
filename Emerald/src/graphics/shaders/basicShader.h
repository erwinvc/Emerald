#pragma once

class BasicShader : public Shader {
public:
	BasicShader() : Shader("Basic", "src/graphics/shaders/basic.vert", "src/graphics/shaders/basic.frag")
	{
		AddUniform("color");
		AddUniform("projection");
	}
};