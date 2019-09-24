#pragma once

class Mesh;

namespace Origin {
	static Vector2 CENTER = Vector2(0, 0);
	static Vector2 LEFT = Vector2(1, 0);
	static Vector2 RIGHT = Vector2(-1, 0);
	static Vector2 TOP = Vector2(0, -1);
	static Vector2 BOTTOM = Vector2(0, 1);
	static Vector2 TOPLEFT = Vector2(1, -1);
	static Vector2 TOPRIGHT = Vector2(-1, -1);
	static Vector2 BOTTOMLEFT = Vector2(1, 1);
	static Vector2 BOTTOMRIGHT = Vector2(-1, 1);
}

class UIRenderer : public Singleton<UIRenderer> {
private:
	shared_ptr<VertexArray> m_vao;
	Mesh* m_quad;
	AssetRef<Shader> m_shader;
	Matrix4 CreateMatrix(Vector2& origin, float rot, float positionX, float positionY, float sizeX, float sizeY, bool flipX, bool flipY);

public:
	UIRenderer() {}
	~UIRenderer();

	void Initialize();
	void Rect(Vector2& origin, float positionX, float positionY, float sizeX, float sizeY, const Color& color = Color::White(), float rot = 0);
	void Rect(Vector2& origin, Vector2& position, Vector2& size, const Color& color = Color::White(), float rot = 0);
	void RenderTexture(AssetRef<Texture> texture, Vector2& origin, float positionX, float positionY, float sizeX, float sizeY, const Color& color = Color::Transparent(), float rot = 0, bool flipX = false, bool flipY = false);
	void RenderTexture(AssetRef<Texture> texture, Vector2& origin, Vector2& position, Vector2& size, const Color& color = Color::Transparent(), float rot = 0, bool flipX = false, bool flipY = false);
};

static UIRenderer* GetUIRenderer() { return UIRenderer::GetInstance(); }