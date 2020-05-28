#pragma once

class VoxelState : public State {
private:
	World* m_world;
	Texture* crosshair;
	Shader* m_directionalShadowShader;
	Shader* m_omniDirectionalShadowShader;
	bool aaa = false;
	
public:
	Pointlight m_pointlight;
	DepthCubemap* m_dcm;
	VoxelState() : State("Voxel") {}

	virtual ~VoxelState() {
		DELETE(m_world);
	}

	void Initialize() override {
		m_world = NEW(World());
		crosshair = GetAssetManager()->Get<Texture>("Crosshair");
		m_dcm = NEW(DepthCubemap());
		m_pointlight = Pointlight(glm::vec3(0, 0, 0), 15, Color::White());
		m_directionalShadowShader = GetShaderManager()->Get("DirectionalShadowChunk");
		m_omniDirectionalShadowShader = GetShaderManager()->Get("DepthCubemap");
	}

	void Update(const TimeStep& time) override {
		Camera::active->Update(time);
		m_world->Update(time);
		if (aaa) m_pointlight.m_position = Camera::active->transform.position;
		if (KeyDown('J'))m_pointlight.m_position.x += time;
		if (KeyDown('L'))m_pointlight.m_position.x -= time;
		if (KeyDown('I'))m_pointlight.m_position.z += time;
		if (KeyDown('K'))m_pointlight.m_position.z -= time;
		if (KeyDown('U'))m_pointlight.m_position.y += time;
		if (KeyDown('O'))m_pointlight.m_position.y -= time;


		float goalAmbient = Math::Clamp(Math::Map(Camera::active->transform.position.y, -8.0f, 0.0f, 0.0f, 0.05f), 0.0f, 1.0f);
		GetClient()->pipeline->m_ambientIntensity = Math::Lerp(GetClient()->pipeline->m_ambientIntensity, goalAmbient, 0.02f);
	}

	void RenderGeometry(HDRPipeline* pipeline) override {
		FirstPersonCam* cam = (FirstPersonCam*)Camera::active;
		cam->DrawUpdate();
		m_world->RenderGeometry(pipeline);
		GetPointlightRenderer()->Submit(m_pointlight);
		pipeline->UIRect(Origin::CENTER, pipeline->Width() / 2.0f, pipeline->Height() / 2.0f, 35, 35, 0, Color::White(), crosshair);
	}

	void RenderGeometryShadow(HDRPipeline* pipeline, ShadowType type) {
		if (type == ShadowType::DIRECTIONAL) {
			m_world->RenderChunks(m_directionalShadowShader);
			GetFrameBufferManager()->BindDefaultFBO();
		} else if (type == ShadowType::OMNIDIRECTIONAL) {
			m_world->RenderChunks(m_omniDirectionalShadowShader);
			GetFrameBufferManager()->BindDefaultFBO();
		}
	}

	void OnStateImGUI() override {
		m_dcm->OnImGui();
		UI::Begin();
		UI::Color4("Pointlight color", &m_pointlight.m_color);
		UI::Float("Size", &m_pointlight.m_radius, 0.0f, 15.0f);
		UI::Bool("AAA", &aaa);
		UI::End();
	}
	void OnImGUI() override {}
	void Cleanup() override {}

	void OnEnterState() override {}
	void OnExitState() override {}
	void OnResize(int width, int height) override {}

	World* GetWorld() { return m_world; }
};

inline World* GetWorld() { return GameStates::VOXEL->GetWorld(); }