#pragma once

class Texture;

class Material {
private:
	AssetRef<Texture> m_albedo;
	AssetRef<Texture> m_normal;
	AssetRef<Texture> m_specular;
	AssetRef<Texture> m_emission;

public:
	float m_normalStrength = 0;
	float m_specularStrength = 0;
	float m_emissionStrength = 0;

	Material();
	~Material();
	AssetRef<Material> SetAlbedo(AssetRef<Texture> albedo) { m_albedo = albedo; return this; }
	AssetRef<Material> SetNormal(AssetRef<Texture> normal) { m_normalStrength = 1; m_normal = normal; return this; }
	AssetRef<Material> SetSpecular(AssetRef<Texture> specular) { m_specularStrength = 1; m_specular = specular; return this; }
	AssetRef<Material> SetEmission(AssetRef<Texture> emission) { m_emissionStrength = 1; m_emission = emission; return this; }
	
	AssetRef<Texture> GetAlbedo() { return m_albedo; }
	AssetRef<Texture> GetNormal() { return m_normal; }
	AssetRef<Texture> GetSpecular() { return m_normal; }
	AssetRef<Texture> GetEmission() { return m_emission; }

	void Bind(AssetRef<Shader> shader, int slot = 0) {
		shader->Set("_Albedo", slot);
		shader->Set("_Normal", slot + 1);
		shader->Set("_NormalStrength", m_normalStrength);
		shader->Set("_Specular", slot + 2);
		shader->Set("_SpecularStrength", m_specularStrength);
		shader->Set("_Emission", slot + 3);
		shader->Set("_EmissionStrength", m_specularStrength);
		m_albedo->Bind(slot);
		m_normal->Bind(slot + 1);
		m_specular->Bind(slot + 2);
		m_emission->Bind(slot + 3);
	}

	void Unbind(uint slot = 0) {
		m_albedo->Unbind(slot);
		m_normal->Unbind(slot + 1);
		m_specular->Unbind(slot + 2);
		m_emission->Unbind(slot + 3);
	}
};