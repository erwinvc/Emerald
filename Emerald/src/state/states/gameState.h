#pragma once

class GameState : public State {
private:
	String m_name = "Game";
	AssetRef<Texture> m_texture;
	Vector2 m_pos;
	Vector2 m_origin = Origin::CENTER;
	Vector2 m_scale = Vector2(100, 100);
public:
	const String& GetName() override { return m_name; }
	void Initialize() override {
		m_texture = GetAssetManager()->Get<Texture>("White");
	}
	void Update(const TimeStep& time) override {
	}
	void RenderGeometry() override {
	}

	void RenderUI() override {
		GetUIRenderer()->RenderTexture(m_texture, m_origin, m_pos, m_scale);
	}

	void OnImGUI() override {
	}

	void Cleanup() override;
	void OnEnterState() override {}
	void OnExitState() override {}
	void OnResize(int width, int height) override {}
};