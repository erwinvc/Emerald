#pragma once

class Mesh;

namespace Origin {
	static glm::vec2 CENTER = glm::vec2(0, 0);
	static glm::vec2 LEFT = glm::vec2(1, 0);
	static glm::vec2 RIGHT = glm::vec2(-1, 0);
	static glm::vec2 TOP = glm::vec2(0, -1);
	static glm::vec2 BOTTOM = glm::vec2(0, 1);
	static glm::vec2 TOPLEFT = glm::vec2(1, -1);
	static glm::vec2 TOPRIGHT = glm::vec2(-1, -1);
	static glm::vec2 BOTTOMLEFT = glm::vec2(1, 1);
	static glm::vec2 BOTTOMRIGHT = glm::vec2(-1, 1);
}

class UIRenderer : public Singleton<UIRenderer> {
private:
	shared_ptr<VertexArray> m_vao;
	Mesh* m_quad = nullptr;
	AssetRef<Shader> m_shader;
	glm::mat4 CreateMatrix(glm::vec2& origin, float rot, float positionX, float positionY, float sizeX, float sizeY, bool flipX, bool flipY);

public:
	UIRenderer() {}
	~UIRenderer();

	void Initialize();
	void Rect(glm::vec2& origin, float positionX, float positionY, float sizeX, float sizeY, const Color& color = Color::White(), float rot = 0);
	void Rect(glm::vec2& origin, glm::vec2& position, glm::vec2& size, const Color& color = Color::White(), float rot = 0);
	void RenderTexture(AssetRef<Texture> texture, glm::vec2& origin, float positionX, float positionY, float sizeX, float sizeY, const Color& color = Color::Transparent(), float rot = 0, bool flipX = false, bool flipY = false);
	void RenderTexture(AssetRef<Texture> texture, glm::vec2& origin, glm::vec2& position, glm::vec2& size, const Color& color = Color::Transparent(), float rot = 0, bool flipX = false, bool flipY = false);
};

static UIRenderer* GetUIRenderer() { return UIRenderer::GetInstance(); }