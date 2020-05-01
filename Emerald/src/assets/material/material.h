#pragma once

//class Material {
//protected:
//	friend class MaterialManager;
//
//public:
//	virtual void Bind(AssetRef<Shader> shader) = 0;
//	//virtual void Unbind() = 0;
//};

class MaterialMember {
public:
	String m_name;
	String m_uniform;
	ShaderPropertyType m_type;
	virtual void Set(Shader* shader) = 0;
	virtual void OnImGui() = 0;
};

class MaterialMemberRGB : public MaterialMember {
public:
	Color m_value;
	void Set(Shader* shader) override {
		shader->Set(m_uniform.c_str(), glm::vec3(m_value.R, m_value.G, m_value.B));
	}

	void OnImGui() override {
		UI::Color3(m_name.c_str(), &m_value);
	}
};

class MaterialMemberRGBA : public MaterialMember {
public:
	Color m_value;
	void Set(Shader* shader) override {
		shader->Set(m_uniform.c_str(), m_value);
	}

	void OnImGui() override {
		UI::Color4(m_name.c_str(), &m_value);
	}
};

class MaterialMemberFloat : public MaterialMember {
public:
	float m_value;
	void Set(Shader* shader) override {
		shader->Set(m_uniform.c_str(), m_value);
	}

	void OnImGui() override {
		UI::Float(m_name.c_str(), &m_value);
	}
};

class MaterialMemberRange : public MaterialMember {
public:
	float m_value;
	float m_min;
	float m_max;
	void Set(Shader* shader) override {
		shader->Set(m_uniform.c_str(), m_value);
	}

	void OnImGui() override {
		UI::Float(m_uniform.c_str(), &m_value, m_min, m_max);
	}
};

class MaterialMemberInt : public MaterialMember {
public:
	int m_value;
	void Set(Shader* shader) override {
		shader->Set(m_uniform.c_str(), m_value);
	}

	void OnImGui() override {
		UI::Int(m_name.c_str(), &m_value);
	}
};

class MaterialMemberBool : public MaterialMember {
public:
	bool m_value;
	void Set(Shader* shader) override {
		shader->Set(m_uniform.c_str(), m_value);
	}

	void OnImGui() override {
		UI::Bool(m_name.c_str(), &m_value);
	}
};

class MaterialMemberVec2 : public MaterialMember {
public:
	glm::vec2 m_value;
	void Set(Shader* shader) override {
		shader->Set(m_uniform.c_str(), m_value);
	}

	void OnImGui() override {
		UI::Vec2(m_name.c_str(), &m_value);
	}
};

class MaterialMemberVec3 : public MaterialMember {
public:
	glm::vec3 m_value;
	void Set(Shader* shader) override {
		shader->Set(m_uniform.c_str(), m_value);
	}

	void OnImGui() override {
		UI::Vec3(m_name.c_str(), &m_value);
	}
};

class MaterialMemberVec4 : public MaterialMember {
public:
	glm::vec4 m_value;
	void Set(Shader* shader) override {
		shader->Set(m_uniform.c_str(), m_value);
	}

	void OnImGui() override {
		UI::Vec4(m_name.c_str(), &m_value);
	}
};

class MaterialMemberTexture : public MaterialMember {
public:
	ShaderPropertyTextureType m_textureType;
	Texture* m_texture;
	int m_slot;
	void Set(Shader* shader) override {
		shader->Set(m_uniform.c_str(), m_slot);
		m_texture->Bind(m_slot);
	}

	void OnImGui() override {
		ImGui::AlignTextToFramePadding();
		ImGui::Text(m_name.c_str());
		float width = ImGui::GetContentRegionAvail().x;
		ImGui::SameLine(width / 2);
		ImGui::PushItemWidth(width / 2);
		ImGui::Image(m_texture->GetImGuiHandle(), ImVec2(120, 120), ImVec2(0, 1), ImVec2(1, 0));
	}
};
class Material {
private:
	String m_name;
	Shader* m_shader;
	vector<MaterialMember*> m_members;
public:
	Material(const String& name, Shader* shader);

	~Material() {
		for (auto& member : m_members) {
			DELETE(member);
		}
		m_members.clear();
	}

	const String& GetName() { return m_name; }

	Shader* GetShader() { return m_shader; }

	void Bind() {
		m_shader->Bind();
		for (auto& member : m_members) {
			member->Set(m_shader);
		}
	}

	void OnImGui() {
		for (auto& member : m_members) {
			member->OnImGui();
		}
	}

	//#Dirty!
	void SetRoughnessIfAvailable(Texture* tex) {
		LOG("Tried to set");
		for (auto& member : m_members) {
			if (member->m_type == ShaderPropertyType::TEXTURE) {
				if (member->m_uniform.compare("_Roughness") == 0) {
					((MaterialMemberTexture*)member)->m_texture = tex;
				}
			}
		}
	}
	void SetAlbedoIfAvailable(Texture* tex) {
		LOG("Tried to set alb");
		for (auto& member : m_members) {
			if (member->m_type == ShaderPropertyType::TEXTURE) {
				if (member->m_uniform.compare("_Albedo") == 0) {
					((MaterialMemberTexture*)member)->m_texture = tex;
				}
			}
		}
	}
	void SetNormalIfAvailable(Texture* tex) {
		for (auto& member : m_members) {
			if (member->m_type == ShaderPropertyType::TEXTURE) {
				if (member->m_uniform.compare("_Normal") == 0) {
					((MaterialMemberTexture*)member)->m_texture = tex;
				}
			}
		}
	}
	void SetMetallicIfAvailable(Texture* tex) {
		for (auto& member : m_members) {
			if (member->m_type == ShaderPropertyType::TEXTURE) {
				if (member->m_uniform.compare("_Metallic") == 0) {
					((MaterialMemberTexture*)member)->m_texture = tex;
				}
			}
		}
	}
};