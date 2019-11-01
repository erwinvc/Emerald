#pragma once

struct Position {
	Vector3 m_position;
};
class MenuState : public State {
private:
	String m_name = "Menu";

	Model* m_mori;
	Entity* m_moriEntity;
	Shader* m_geometryShader;
	Texture* iri;
	Texture* noise;
	BasicMaterial* mat;
	//Model* sponza;
	Entity* sponzaEntity;
	vector<Pointlight> m_pointLights;
	vector<Vector3> m_positions;
	int LIGHTCOUNT = 16000;
public:
	const String& GetName() override { return m_name; }

	void Initialize() override {
		m_mori = GetAssetManager()->Get<Model>("Mori");
		mat = GetMaterialManager()->Create<BasicMaterial>("MoriMaterial");
		mat->SetPBR("planks");
		m_mori->SetMaterial(mat);
		m_moriEntity = new Entity(m_mori);
		m_geometryShader = GetShaderManager()->Get("Geometry");
		iri = GetAssetManager()->Get<Texture>("Iridescence");
		noise = GetAssetManager()->Get<Texture>("Noise");
		//sponza = GetAssetManager()->Get<Model>("Sponza");
		Model* model = new Model();
		model->LoadModel("res/sponza/sponza.obj");
		sponzaEntity = new Entity(model);
		sponzaEntity->m_transform.m_size = Vector3(0.1f, 0.1f, 0.1f);
		sponzaEntity->m_transform.m_position.z = 4;
		//GetCamera()->m_position.y = -2;
		GetCamera()->m_position = Vector3(-1.3f, 0.96f, 0);
		GetCamera()->m_rotation.y = Math::HALF_PI;
		m_moriEntity->m_transform.m_position.y = 0.5f;
		GetPipeline()->m_directionalLight.m_multiplier = 5.0f;
		for (int i = 0; i < LIGHTCOUNT; i++) {
			m_positions.push_back(Vector3(Math::RandomFloat(-150.0f, 130.0f), Math::RandomFloat(0.0f, 90.0f), Math::RandomFloat(-60.0f, 60.0f)));
		}
	}
	bool started = false;
	bool disableMori = false;
	bool moveLights = false;
	bool irid = false;
	int amount = 0;
	void Update(const TimeStep& time) override {
		GetCamera()->Update(time);
		static float seconds = 0;
		seconds += time.GetSeconds();
		m_moriEntity->m_transform.m_rotation.y += Math::QUARTER_PI * time.GetSeconds();

		if (moveLights) {
			amount = Math::Clamp(amount + 5, 0, LIGHTCOUNT);
			for (int i = 0; i < amount; i++) {
				m_pointLights[i].m_position.Lerp(m_positions[i], time.GetSeconds() / 4.0f);
			}
		}
	}
	void RenderGeometry() override {
		m_geometryShader->Bind();
		m_geometryShader->Set("_Iridescence", 5);
		m_geometryShader->Set("_Noise", 6);
		iri->Bind(5);
		noise->Bind(6);
		m_geometryShader->Set("_IridescenceStrength", 0);
		m_geometryShader->Set("_ProjectionMatrix", GetCamera()->GetProjectionMatrix());
		m_geometryShader->Set("_ViewMatrix", GetCamera()->GetViewMatrix());
		m_geometryShader->Set("_TransformationMatrix", Matrix4::Identity());
		m_geometryShader->Set("_CameraPosition", GetCamera()->m_position);

		m_geometryShader->Set("Iri", irid ? 1.0f : 0.0f);
		if (!disableMori)m_moriEntity->Draw(m_geometryShader);
		m_geometryShader->Set("Iri", 0);

		sponzaEntity->Draw(m_geometryShader);

		GetPointlightRenderer()->Submit(m_pointLights.data(), m_pointLights.size());

	}
	void RenderUI() override {}
	void OnStateImGUI() override {
		if (ImGui::Button("Planks")) mat->SetPBR("planks");
		if (ImGui::Button("Gold")) mat->SetPBR("gold");
		if (ImGui::Button("Metal")) mat->SetPBR("metal");
		static float temp = 0;

		if (ImGui::Button("Start")) {
			if (!started) {
				Tween::To(temp, 0.0f, 5000)->SetOnComplete([&] {
					mat->SetPBR("gold");

					Tween::To(temp, 0.0f, 5000)->SetOnComplete([&] {
						mat->SetPBR("metal");
						Tween::To(temp, 0.0f, 5000)->SetOnComplete([&] {
							irid = true;
							Tween::To(temp, 0.0f, 5000)->SetOnComplete([&] {

								Tween::To(GetCamera()->m_position, Vector3(-105, 32, 0), 5000)->SetEase(Ease::INOUTEXPO);
								Tween::To(GetCamera()->m_rotation, Vector3(0.2f, Math::HALF_PI, 0), 5000)->SetEase(Ease::INOUTEXPO);
								Tween::To(temp, 0.0f, 4000)->SetOnComplete([&] {
									disableMori = true;
									GetPipeline()->m_applyPostProcessing = false;
									GetFrameBufferManager()->SetSelectedTexture(GetPipeline()->GetGBuffer()->m_colorTexture);

									Tween::To(temp, 0.0f, 4000)->SetOnComplete([&] {
										GetFrameBufferManager()->SetSelectedTexture(GetPipeline()->GetGBuffer()->m_normalTexture);

										Tween::To(temp, 0.0f, 4000)->SetOnComplete([&] {
											GetFrameBufferManager()->SetSelectedTexture(GetPipeline()->GetGBuffer()->m_attributesTexture);

											Tween::To(temp, 0.0f, 4000)->SetOnComplete([&] {
												GetFrameBufferManager()->SetSelectedTexture(GetPipeline()->m_ssaoRenderer->GetRawTexture());

												Tween::To(temp, 0.0f, 4000)->SetOnComplete([&] {
													GetPipeline()->m_applyPostProcessing = true;
													GetFrameBufferManager()->SetSelectedTexture(GetPipeline()->GetHDRTexture());

													Tween::To(GetPipeline()->m_ambientIntensity, 0.01f, 2500);
													Tween::To(GetPipeline()->m_directionalLight.m_multiplier, 0.0f, 2500);
													Tween::To(temp, 0.0f, 2000)->SetOnComplete([&] {

														for (int i = 0; i < LIGHTCOUNT; i++) {
															m_pointLights.push_back(Pointlight(Vector3(0, 2, 0), 5, Color::RandomPrimary()));
														}
														Tween::To(temp, 0.0f, 1000)->SetOnComplete([&] {
															moveLights = true;
															Tween::To(GetPipeline()->m_exposure, 4.0f, 1000)->SetOnComplete([&] {

															});
														});
													});
												});
											});
										});
									});
								});
							});
						});
					});
				});
			}
			started = true;
		}
	}
	void OnImGUI() override {
	}
	void Cleanup() override {}

	void OnEnterState() override {}
	void OnExitState() override {}
	void OnResize(int width, int height) override {}
};