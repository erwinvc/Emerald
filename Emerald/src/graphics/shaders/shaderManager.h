#pragma once

class ShaderManager : public Singleton<ShaderManager> {
private:
	map<String, AssetRef<Shader>> m_shaders;
	vector<AssetRef<Shader>> m_shadersVector;
	ShaderManager() {}
	~ShaderManager() {}
	friend Singleton;

public:

	AssetRef<Shader> Create(const String& name, const String& file, bool test) {
		if (m_shaders[name]) {
			LOG("[~bShaders~x] ~1%s ~rshader has already been created", name.c_str());
			return m_shaders[name];
		}
		AssetRef<Shader> shader = NEW(Shader(name, file, test));
		m_shaders[name] = shader;
		m_shadersVector.push_back(shader);
		return shader;
	}

	AssetRef<Shader> Get(String name) {
		return m_shaders[name];
	}

	void Update(const TimeStep& time) {

	}

	void OnImGUI() {
		if (ImGui::BeginTabItem("Shaders")) {
			int i = 0;
			for (AssetRef<Shader>& shader : m_shadersVector) {
				ImGui::Text(shader->m_name.c_str());
				if (ImGui::Button(Format_t("Reload##%d", i++))) {
					shader->Reload();
				}
				ImGui::Separator();
			}
			ImGui::EndTabItem();
		}
	}
};

static ShaderManager* GetShaderManager() { return ShaderManager::GetInstance(); }