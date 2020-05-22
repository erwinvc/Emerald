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

	//void Rect(glm::vec2 origin, float x, float y, float w, float h, float uMin, float uMax, float vMin, float vMax, float rotation = 0, const Color& color = Color::White(), Texture* texture = nullptr, const glm::vec3& atlasValues = glm::vec3(1.0f, 0.0f, 0.0f)) {
	//	float textureSlot = 0.0f;
	//	if (texture) textureSlot = SubmitTexture(texture);
	//	Vertex vertices[4];
	//
	//	//glm::mat4 transform = glm::translate(glm::mat4(1.0f), -glm::vec3(origin.x, origin.y, 0));
	//	//transform = glm::rotate(transform, rotation, glm::vec3(0.0f, 0.0f, 1.0f));
	//	//transform = glm::translate(transform, glm::vec3(origin.x, origin.y, 0));
	//
	//	glm::mat4 rot = glm::rotate(rotation, glm::vec3(0.0f, 0.0f, 1.0f));
	//	//Top left
	//	vertices[0].m_position = glm::vec3(x, y, 0) + glm::mat3(rot) * glm::vec3(-w / 2.0f, h / 2.0f, 0.0f);
	//	vertices[0].m_uv = glm::vec2(uMin, vMax);
	//	vertices[0].m_textureID = textureSlot;
	//	vertices[0].m_color = color;
	//	vertices[0].vsAtlasValues = atlasValues;
	//	//Top right
	//	vertices[1].m_position = glm::vec3(x, y, 0) + glm::mat3(rot) * glm::vec3(w / 2.0f, h / 2.0f, 0.0f);
	//	vertices[1].m_uv = glm::vec2(uMax, vMax);
	//	vertices[1].m_textureID = textureSlot;
	//	vertices[1].m_color = color;
	//	vertices[1].vsAtlasValues = atlasValues;
	//	//Bottom right
	//	vertices[2].m_position = glm::vec3(x, y, 0) + glm::mat3(rot) * glm::vec3(w / 2.0f, -h / 2.0f, 0.0f);
	//	vertices[2].m_uv = glm::vec2(uMax, vMin);
	//	vertices[2].m_textureID = textureSlot;
	//	vertices[2].m_color = color;
	//	vertices[2].vsAtlasValues = atlasValues;
	//	//Bottom left
	//	vertices[3].m_position = glm::vec3(x, y, 0) + glm::mat3(rot) * glm::vec3(-w / 2.0f, -h / 2.0f, 0.0f);
	//	vertices[3].m_uv = glm::vec2(uMin, vMin);
	//	vertices[3].m_textureID = textureSlot;
	//	vertices[3].m_color = color;
	//	vertices[3].vsAtlasValues = atlasValues;
	//
	//	m_instancedRenderer->Submit(&vertices[0], 4, 1);
	//}

	void Rect(glm::vec2 origin, float x, float y, float w, float h, float rotation, const Color& color, Texture* texture = nullptr, const glm::vec3& atlasValues = glm::vec3(1.0f, 0.0f, 0.0f)) {
		float textureSlot = 0.0f;
		if (texture) textureSlot = SubmitTexture(texture);
		Vertex vertices[4];

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), -glm::vec3(origin.x, origin.y, 0));
		transform = glm::translate(transform, glm::vec3(x, y, 0.0f));
		transform = glm::rotate(transform, rotation, glm::vec3(0.0f, 0.0f, 1.0f));
		transform = glm::scale(transform, glm::vec3(w / 2.0f, h / 2.0f, 0));
		transform = glm::translate(transform, glm::vec3(origin.x, origin.y, 0));

		//Top left
		vertices[0].m_position = glm::vec3(transform * glm::vec4(-1.0f, 1.0f, 0.0f, 1.0));
		vertices[0].m_uv = glm::vec2(0.0f, 1.0f);
		vertices[0].m_textureID = textureSlot;
		vertices[0].m_color = color;
		vertices[0].vsAtlasValues = atlasValues;
		//Top right
		vertices[1].m_position = glm::vec3(transform * glm::vec4(1.0f, 1.0f, 0.0f, 1.0));
		vertices[1].m_uv = glm::vec2(1.0f, 1.0f);
		vertices[1].m_textureID = textureSlot;
		vertices[1].m_color = color;
		vertices[1].vsAtlasValues = atlasValues;
		//Bottom right
		vertices[2].m_position = glm::vec3(transform * glm::vec4(1.0f, -1.0f, 0.0f, 1.0));
		vertices[2].m_uv = glm::vec2(1.0f, 0.0f);
		vertices[2].m_textureID = textureSlot;
		vertices[2].m_color = color;
		vertices[2].vsAtlasValues = atlasValues;
		//Bottom left
		vertices[3].m_position = glm::vec3(transform * glm::vec4(-1.0f, -1.0f, 0.0f, 1.0));
		vertices[3].m_uv = glm::vec2(0.0f, 0.0f);
		vertices[3].m_textureID = textureSlot;
		vertices[3].m_color = color;
		vertices[3].vsAtlasValues = atlasValues;

		m_elementsRenderer->Submit(&vertices[0], VERTEX_COUNT, 1);
	}

	void Line(float x0, float y0, float x1, float y1, Color& color = Color::White(), float size = 1.0f) {
		float textureSlot = 0.0f;
		glm::vec2 normal = glm::normalize(glm::vec2(y1 - y0, -(x1 - x0))) * glm::vec2(size, size);
		Vertex vertices[4];

		
		vertices[2].m_position = glm::vec3(x0 + normal.x, y0 + normal.y, 0.0f);
		//vertices[0].m_position = glm::vec3(x0 + normal.x, y0 + normal.y, 0.0f);
		vertices[0].m_uv = glm::vec2(0.0f, 1.0f);
		vertices[0].m_textureID = textureSlot;
		vertices[0].m_color = color;
		vertices[0].vsAtlasValues = glm::vec3(1, 0, 0);

		vertices[1].m_position = glm::vec3(x1 + normal.x, y1 + normal.y, 0.0f);
		//vertices[1].m_position = glm::vec3(1920.0f, 1080.0f, 0.0f);
		vertices[1].m_uv = glm::vec2(1.0f, 1.0f);
		vertices[1].m_textureID = textureSlot;
		vertices[1].m_color = color;
		vertices[1].vsAtlasValues = glm::vec3(1, 0, 0);

		vertices[0].m_position = glm::vec3(x1 - normal.x, y1 - normal.y, 0.0f);
		//vertices[2].m_position = glm::vec3(1920.0f, 0.0f, 0.0f);
		vertices[2].m_uv = glm::vec2(1.0f, 0.0f);
		vertices[2].m_textureID = textureSlot;
		vertices[2].m_color = color;
		vertices[2].vsAtlasValues = glm::vec3(1, 0, 0);

		vertices[3].m_position = glm::vec3(x0 - normal.x, y0 - normal.y, 0.0f);
		//vertices[3].m_position = glm::vec3(0.0f, 0.0f, 0.0f);
		vertices[3].m_uv = glm::vec2(0.0f, 0.0f);
		vertices[3].m_textureID = textureSlot;
		vertices[3].m_color = color;
		vertices[3].vsAtlasValues = glm::vec3(1, 0, 0);

		m_elementsRenderer->Submit(&vertices[0], VERTEX_COUNT, 1);
	}

	void LineRect(Rectangle& rect, Color& color = Color::White(), float lineSize = 1.0f) {
		float halfLineSize = lineSize / 2;
		Line(rect.XMin() - halfLineSize, rect.YMin() - halfLineSize, rect.XMin() - halfLineSize, rect.YMax() + halfLineSize, color, lineSize);
		Line(rect.XMax() + halfLineSize, rect.YMin() - halfLineSize, rect.XMax() + halfLineSize, rect.YMax() + halfLineSize, color, lineSize);
		Line(rect.XMin() - halfLineSize, rect.YMin() - halfLineSize, rect.XMax() + halfLineSize, rect.YMin() - halfLineSize, color, lineSize);
		Line(rect.XMin() - halfLineSize, rect.YMax() + halfLineSize, rect.XMax() + halfLineSize, rect.YMax() + halfLineSize, color, lineSize);
	}

	void Begin();

	void End() {
		m_elementsRenderer->End();
	}

	void Draw();
private:
	float SubmitTexture(Texture* texture) {
		float result = 0.0f;
		bool found = false;
		for (uint i = 0; i < m_textures.size(); i++) {
			if (m_textures[i] == texture) {
				result = (float)(i);
				found = true;
				break;
			}
		}

		if (!found) {
			if (m_textures.size() >= 31) {
				//#TODO: Flush
			}
			m_textures.push_back(texture);
			result = (float)(m_textures.size() - 1);
		}
		return result;
	}
};