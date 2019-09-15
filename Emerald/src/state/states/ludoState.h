#pragma once

class LudoState : public State {
private:
	String m_name = "Ludo";
	Ludo* m_ludo;
	AssetRef<Texture> m_sphere;

	Vector2 origin = Vector2(0, 0);
	float rot = 0;
	Vector2 position = Vector2(0, 0);
	Vector2 sizee = Vector2(1, 1);
public:
	const String& GetName() override { return m_name; }

	void Initialize() override {
		m_ludo = new Ludo();
		m_sphere = GetAssetManager()->Get<Texture>("Sphere");
	}

	void Update(const TimeStep& time) override {

	}

	void RenderGeometry() override {}
	void RenderUI() override {
		GetUIRenderer()->RenderTexture(m_sphere, Vector2(0, 0), Vector2(0.5f, 0.5f), Vector2(0.5f, 0.5f));
		GetUIRenderer()->RenderTexture(m_sphere, Origin::CENTER, position, sizee);
	}

	void OnImGUI() override {
		ImGui::SliderFloat2("origin", (float*)&origin, -1, 1);
		ImGui::SliderFloat("rot", (float*)&rot, 0, Math::PI);
		ImGui::SliderFloat2("position", (float*)&position, 0, 1000);
		ImGui::SliderFloat2("size", (float*)&sizee, 0, 1000);
	}

	void Cleanup() override {}

	void OnEnterState() override {}
	void OnExitState() override {}
	void OnResize(int width, int height) override {}
};