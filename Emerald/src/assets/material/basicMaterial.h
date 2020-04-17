#pragma once

//class Texture;
//
//class BasicMaterial : public Material {
//private:
//	AssetRef<Texture> m_albedo;
//	AssetRef<Texture> m_normal;
//	AssetRef<Texture> m_roughness;
//	AssetRef<Texture> m_metallic;
//	AssetRef<Texture> m_emission;
//
//protected:
//	friend class MaterialManager;
//	~BasicMaterial();
//	BasicMaterial();
//
//public:
//	float m_normalStrength = 1;
//	float m_roughnessStrength = 1;
//	float m_metallicStrength = 1;
//	float m_emissionStrength = 1;
//
//	void SetPBR(const String& name);
//
//	AssetRef<BasicMaterial> SetAlbedo(AssetRef<Texture> albedo) { if (albedo.Get() != nullptr) m_albedo = albedo; return this; }
//	AssetRef<BasicMaterial> SetNormal(AssetRef<Texture> normal) { if (normal.Get() != nullptr) { m_normalStrength = 1; m_normal = normal; } return this; }
//	AssetRef<BasicMaterial> SetRoughness(AssetRef<Texture> specular) { if (specular.Get() != nullptr) { m_roughnessStrength = 1; m_roughness = specular; } return this; }
//	AssetRef<BasicMaterial> SetMetallic(AssetRef<Texture> metallic) { if (metallic.Get() != nullptr) { m_metallicStrength = 1; m_metallic = metallic; } return this; }
//	AssetRef<BasicMaterial> SetEmission(AssetRef<Texture> emission) { if (emission.Get() != nullptr) { m_emissionStrength = 1; m_emission = emission; } return this; }
//
//	AssetRef<Texture> GetAlbedo() { return m_albedo; }
//	AssetRef<Texture> GetNormal() { return m_normal; }
//	AssetRef<Texture> GetRoughness() { return m_roughness; }
//	AssetRef<Texture> GetMetallic() { return m_metallic; }
//	AssetRef<Texture> GetEmission() { return m_emission; }
//
//	void Bind(AssetRef<Shader> shader) override {
//		shader->Set("_Albedo", 0);
//		shader->Set("_Normal", 1);
//		shader->Set("_NormalStrength", m_normalStrength);
//		shader->Set("_Roughness", 2);
//		shader->Set("_RoughnessStrength", m_roughnessStrength);
//		shader->Set("_Metallic", 3);
//		shader->Set("_MetallicStrength", m_metallicStrength);
//		shader->Set("_Emission", 4);
//		shader->Set("_EmissionStrength", m_emissionStrength);
//		m_albedo->Bind(0);
//		m_normal->Bind(1);
//		m_roughness->Bind(2);
//		m_metallic->Bind(3);
//		m_emission->Bind(4);
//	}
//
//	//void Unbind() override {
//	//	m_albedo->Unbind(0);
//	//	m_normal->Unbind(1);
//	//	m_specular->Unbind(2);
//	//	m_emission->Unbind(3);
//	//}
//};