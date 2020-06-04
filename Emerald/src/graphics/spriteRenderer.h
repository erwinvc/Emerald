#pragma once

class HDRPipeline;

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

class SpriteRenderer {
public:
	struct Vertex {
		glm::vec3 m_position;
		glm::vec2 m_uv;
		float m_textureID;
		Color m_color;
		glm::vec3 vsAtlasValues;
	};

	vector<Texture*> m_textures;
	const int MAX_SPRITES = 64;
	const int INDICES_COUNT = 6;
	const int VERTEX_COUNT = 4;
	const int INDICES_SIZE = MAX_SPRITES * INDICES_COUNT;
	const int MAX_TEXTURES = 32 - 1;

	BufferLayout m_layout = {
		{VertexBufferDataType::Float3, "vsPos", 0},
		{VertexBufferDataType::Float2, "vsUv", 0},
		{VertexBufferDataType::Float, "vsTextureID", 0},
		{VertexBufferDataType::Float4, "vsColor", 0},
		{VertexBufferDataType::Float3, "vsAtlasValues", 0}
	};

	Shader* m_shader;

	ElementsRenderer<Vertex>* m_elementsRenderer;

	SpriteRenderer();

	~SpriteRenderer() {
		delete m_elementsRenderer;
	}
	
	void Rect(glm::vec2 origin, float x, float y, float w, float h, float rotation, const Color& color, Texture* texture = nullptr, const glm::vec3& atlasValues = glm::vec3(1.0f, 0.0f, 0.0f));
	void Line(float x0, float y0, float x1, float y1, Color& color = Color::White(), float size = 1.0f);
	void LineRect(Rectangle& rect, Color& color = Color::White(), float lineSize = 1.0f);

	void Begin();
	void End();
	void Draw();
	
private:
	float SubmitTexture(Texture* texture);
};