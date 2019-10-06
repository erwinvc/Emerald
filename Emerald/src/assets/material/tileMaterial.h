#pragma once

class TileMaterial : public Material {
private:
	int m_textureID;
public:
	TileMaterial(int id) : m_textureID(id) {}
	void Bind(AssetRef<Shader> shader) override {}
	void GetTextureID() {

	}
};