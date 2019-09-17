#pragma once

class Texture;

class BasicMaterial : public Material {
private:
	AssetRef<Texture> m_albedo;
	AssetRef<Texture> m_normal;
	AssetRef<Texture> m_specular;
	AssetRef<Texture> m_emission;

protected:
	friend class MaterialManager;
	~BasicMaterial();
	BasicMaterial();

public:
	float m_normalStrength = 1;
	float m_specularStrength = 1;
	float m_emissionStrength = 1;

	AssetRef<BasicMaterial> SetAlbedo(AssetRef<Texture> albedo) { if (albedo.Get() != nullptr) m_albedo = albedo; return this; }
	AssetRef<BasicMaterial> SetNormal(AssetRef<Texture> normal) { if (normal.Get() != nullptr) { m_normalStrength = 1; m_normal = normal; } return this; }
	AssetRef<BasicMaterial> SetSpecular(AssetRef<Texture> specular) { if (specular.Get() != nullptr) { m_specularStrength = 1; m_specular = specular; } return this; }
	AssetRef<BasicMaterial> SetEmission(AssetRef<Texture> emission) { if (emission.Get() != nullptr) { m_emissionStrength = 1; m_emission = emission; } return this; }

	AssetRef<Texture> GetAlbedo() { return m_albedo; }
	AssetRef<Texture> GetNormal() { return m_normal; }
	AssetRef<Texture> GetSpecular() { return m_normal; }
	AssetRef<Texture> GetEmission() { return m_emission; }

	void Bind(AssetRef<Shader> shader) override {
		shader->Set("_Albedo", 0);
		shader->Set("_Normal", 1);
		shader->Set("_NormalStrength", m_normalStrength);
		shader->Set("_Specular", 2);
		shader->Set("_SpecularStrength", m_specularStrength);
		shader->Set("_Emission", 3);
		shader->Set("_EmissionStrength", m_emissionStrength);
		m_albedo->Bind(0);
		m_normal->Bind(1);
		m_specular->Bind(2);
		m_emission->Bind(3);
	}

	void Unbind() override {
		m_albedo->Unbind(0);
		m_normal->Unbind(1);
		m_specular->Unbind(2);
		m_emission->Unbind(3);
	}
};