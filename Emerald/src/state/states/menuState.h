#pragma once

struct Position {
	glm::vec3 m_position;
};
class MenuState : public State {
private:
	Model* m_mori;
	Entity* m_moriEntity;
	Shader* m_geometryShader;
	Texture* iri;
	Texture* noise;
	Material* mat;
	//Model* sponza;
	Entity* sponzaEntity;
	vector<Pointlight> m_pointLights;
	vector<glm::vec3> m_positions;
	int LIGHTCOUNT = 8000;
	
public:
	MenuState() : State("Menu") {}
	

	void Initialize() override {
		//m_pointLights.push_back(Pointlight(glm::vec3(0, 3, 0), 15, Color::Blue()));
		//m_geometryShader = GetShaderManager()->Get("Geometry");
		//m_mori = GetAssetManager()->Get<Model>("wheelbarrow");
		//mat = GetMaterialManager()->Create("MoriMaterial", m_geometryShader);
		//mat->SetPBR("wheelbarrow");
		//m_mori->SetMaterial(mat);
		//m_moriEntity = new Entity(m_mori);
		//iri = GetAssetManager()->Get<Texture>("Iridescence");
		//noise = GetAssetManager()->Get<Texture>("Noise");
		////sponza = GetAssetManager()->Get<Model>("Sponza");
		//Model* model = GetAssetManager()->Get<Model>("Sponza");
		//sponzaEntity = new Entity(model);
		//sponzaEntity->m_transform.m_size = glm::vec3(0.1f, 0.1f, 0.1f);
		//
		////GetCamera()->m_position.y = -2;
		//Camera::active->transform.position = glm::vec3(-1.3f, 0.96f, 0);
		//Camera::active->transform.rotation.y = Math::HALF_PI;
		////m_moriEntity->m_transform.position.y = 5.0f;
		////m_moriEntity->m_transform.position.x = -6.0f;
		//m_moriEntity->m_transform.m_size = glm::vec3(0.2f);
		////GetPipeline()->m_directionalLight.m_multiplier = 5.0f;
		//for (int i = 0; i < LIGHTCOUNT; i++) {
		//	m_positions.push_back(glm::vec3(Random::Float(-150.0f, 130.0f), Random::Float(0.0f, 90.0f), Random::Float(-60.0f, 60.0f)));
		//}
	}
	bool started = false;
	bool disableMori = false;
	bool moveLights = false;
	bool irid = false;
	int amount = 0;
	void Update(const TimeStep& time) override {
		Camera::active->Update(time);
		static float seconds = 0;
		seconds += time.DeltaTime();
		m_moriEntity->transform.rotation.y += Math::QUARTER_PI * time.DeltaTime();

		if (moveLights) {
			amount = Math::Clamp(amount + 5, 0, LIGHTCOUNT);
			for (int i = 0; i < amount; i++) {
				//m_pointLights[i].m_position.Lerp(m_positions[i], time.GetSeconds() / 4.0f);
				m_pointLights[i].m_position = glm::lerp(m_pointLights[i].m_position, m_positions[i], time.DeltaTime() / 4.0f);
			}
		}

		if (GetMouse()->ButtonJustDown(VK_MOUSE_MIDDLE)) {
			m_pointLights.push_back(Pointlight(Camera::active->transform.position, 2, Color::RandomPrimary() * 2));

		}
	}
	void RenderGeometry(HDRPipeline* pipeline) override {
		m_geometryShader->Bind();

		//if (!disableMori) m_moriEntity->Draw();
		m_moriEntity->Draw();
		sponzaEntity->Draw();


		//Ray ray(Camera::active->transform.m_position, Camera::active->transform.m_rotation);
		//for (; ray.GetLength() < 8; ray.Step(0.25f)) {
		//	auto rayBlockPosition = Chunk::ToBlockPosition(ray.GetEndpoint());
		//	if (m_chunk.GetBlock(rayBlockPosition) == 1) {
		//		pipeline->Bounds(glm::vec3(rayBlockPosition) + glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.501f, 0.501f, 0.501f), Color(0.0f, 0.0f, 0.0f, 0.55f), false);
		//		break;
		//	}
		//}
		GetPointlightRenderer()->Submit(m_pointLights.data(), m_pointLights.size());
	}

	void OnStateImGUI() override {
		static int currentlySelectedShader = 0;

		if (ImGui::BeginCombo("Shader", sponzaEntity->m_model->GetMeshes()[currentlySelectedShader]->GetMaterial()->GetName().c_str())) {
			for (int n = 0; n < sponzaEntity->m_model->GetMeshes().size(); n++) {
				bool is_selected = (currentlySelectedShader == n);
				if (ImGui::Selectable(sponzaEntity->m_model->GetMeshes()[n]->GetMaterial()->GetName().c_str(), is_selected))
					currentlySelectedShader = n;
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		//sponzaEntity->m_model->GetMaterials()[currentlySelectedShader]->OnImGui();
		ImGui::Separator();

		if (ImGui::Button("Planks")) mat->SetPBR("planks");
		if (ImGui::Button("Gold")) mat->SetPBR("gold");
		if (ImGui::Button("Metal")) mat->SetPBR("metal");
		ImGui::DragFloat3("pos", (float*)&sponzaEntity->transform.position);
		ImGui::DragFloat3("rot", (float*)&sponzaEntity->transform.rotation, 0.02f);
		ImGui::DragFloat3("size", (float*)&sponzaEntity->transform.size, 0.02f);

		ImGui::DragFloat3("pos1", (float*)&m_moriEntity->transform.position);
		ImGui::DragFloat3("rot1", (float*)&m_moriEntity->transform.rotation, 0.02f);
		ImGui::DragFloat3("size1", (float*)&m_moriEntity->transform.size, 0.02f);
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

								Tween::To(Camera::active->transform.position, glm::vec3(-105, 32, 0), 5000)->SetEase(Ease::INOUTEXPO);
								Tween::To(Camera::active->transform.rotation, glm::vec3(0.2f, Math::HALF_PI, 0), 5000)->SetEase(Ease::INOUTEXPO);
								Tween::To(temp, 0.0f, 4000)->SetOnComplete([&] {
									disableMori = true;
									GetApp()->pipeline->m_applyPostProcessing = false;
									GetFrameBufferManager()->SetSelectedTexture(GetApp()->pipeline->GetGBuffer()->m_colorTexture);

									Tween::To(temp, 0.0f, 4000)->SetOnComplete([&] {
										GetFrameBufferManager()->SetSelectedTexture(GetApp()->pipeline->GetGBuffer()->m_normalTexture);

										Tween::To(temp, 0.0f, 4000)->SetOnComplete([&] {
											GetFrameBufferManager()->SetSelectedTexture(GetApp()->pipeline->GetGBuffer()->m_attributesTexture);

											Tween::To(temp, 0.0f, 4000)->SetOnComplete([&] {
												GetFrameBufferManager()->SetSelectedTexture(GetApp()->pipeline->m_ssaoRenderer->GetRawTexture());

												Tween::To(temp, 0.0f, 4000)->SetOnComplete([&] {
													GetApp()->pipeline->m_applyPostProcessing = true;
													GetFrameBufferManager()->SetSelectedTexture(GetApp()->pipeline->GetHDRTexture());

													Tween::To(GetApp()->pipeline->m_ambientIntensity, 0.01f, 2500);
													Tween::To(GetApp()->pipeline->m_directionalLight.m_multiplier, 0.0f, 2500);
													Tween::To(temp, 0.0f, 2000)->SetOnComplete([&] {

														for (int i = 0; i < LIGHTCOUNT; i++) {
															m_pointLights.push_back(Pointlight(glm::vec3(0, 2, 0), Random::Float(5.0f, 15.0f), Color::RandomPrimary()));
														}
														Tween::To(temp, 0.0f, 1000)->SetOnComplete([&] {
															moveLights = true;
															Tween::To(GetApp()->pipeline->m_exposure, 4.0f, 1000)->SetOnComplete([&] {

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