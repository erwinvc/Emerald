#pragma once

class GameState : public State {
private:
	String m_name = "Game";
	AssetRef<Texture> m_texture;
	Vector2 m_pos;
	Vector2 m_origin = Origin::CENTER;
	Vector2 m_scale = Vector2(100, 100);
	Model* m_cube;
	AssetRef<Shader> m_geometryShader;
	Texture* texIri;
	BasicMaterial* mat;
	Pointlight pl;
	Texture* m_height;
	float rotation = 0;
	float heighStrength = 0;
	float r1 = 1;
	float r2 = 1;
	float r3 = 1;
	float r4 = 1;
public:
	const String& GetName() override { return m_name; }
	void Initialize() override {
		m_texture = GetAssetManager()->Get<Texture>("White");
		m_cube = GetAssetManager()->Get<Model>("Cube");
		mat = GetMaterialManager()->Create<BasicMaterial>("Cube");
		mat->SetAlbedo(GetAssetManager()->Get<Texture>("RustAlbedo"));
		mat->SetNormal(GetAssetManager()->Get<Texture>("RustNormal"));
		mat->SetSpecular(GetAssetManager()->Get<Texture>("RustRoughness"));
		mat->SetMetallic(GetAssetManager()->Get<Texture>("RustMetallic"));
		m_cube->SetMaterial(mat);
		m_height = GetAssetManager()->Get<Texture>("RustHeight");
		texIri = GetAssetManager()->Get<Texture>("Irridescence");
		m_geometryShader = GetShaderManager()->Get("Geometry");
		pl = Pointlight(Vector3(0, 1.25f, 0), 10, Color::Blue());
	}


	void Update(const TimeStep& time) override {
		GetCamera()->Update(time);
		static float sinVal = 0;
		sinVal += time * 0.0005f;
		pl.m_position.x = Math::Sin(rotation);
		pl.m_position.z = Math::Cos(rotation);
	}
	void RenderGeometry() override {
		m_geometryShader->Bind();
		m_geometryShader->Set("_Iridescence", 5);
		texIri->Bind(5);
		m_geometryShader->Set("_IridescenceStrength", 0);
		m_geometryShader->Set("_ProjectionMatrix", GetCamera()->GetProjectionMatrix());
		m_geometryShader->Set("_ViewMatrix", GetCamera()->GetViewMatrix());
		m_geometryShader->Set("_TransformationMatrix", Matrix4::Identity());
		m_geometryShader->Set("_CameraPosition", GetCamera()->m_position);
		m_geometryShader->Set("_TessellationFactor", (float)r1);
		m_geometryShader->Set("_TessellationSlope", (float)r2);
		m_geometryShader->Set("_TessellationShift", (float)r3);
		m_geometryShader->Set("_TessellationAlpha", (float)r4);
		m_geometryShader->Set("_HeightStrength", (float)heighStrength);
		m_geometryShader->Set("_Height", 6);
		m_height->Bind(6);
		m_cube->Draw(m_geometryShader, GL_PATCHES);

		GetPointlightRenderer()->Submit(pl);
	}

	void RenderUI() override {
		GetUIRenderer()->RenderTexture(m_texture, m_origin, m_pos, m_scale);
	}

	void OnImGUI() override {
		ImGui::SliderFloat("Specular", &mat->m_specularStrength, 0, 1);
		ImGui::SliderFloat("Metallic", &mat->m_metallicStrength, 0, 1);
		ImGui::SliderFloat("Emission", &mat->m_emissionStrength, 0, 1);
		ImGui::SliderFloat("Normal", &mat->m_normalStrength, -5, 5);
		ImGui::SliderFloat("Rotation", &rotation, 0, Math::TWO_PI);
		ImGui::SliderFloat("Height strength", &heighStrength, 0, 1);

		ImGui::SliderFloat("TessellationFactor", &r1, 0, 10);
		ImGui::SliderFloat("TessellationSlope", &r2, 0, 10);
		ImGui::SliderFloat("TessellationShift", &r3, 0, 10);
		ImGui::SliderFloat("TessellationAlpha", &r4, 0, 1);
	}

	void Cleanup() override;
	void OnEnterState() override {}
	void OnExitState() override {}
	void OnResize(int width, int height) override {}
};