#pragma once

class HDRPipeline;
class Sprite {
private:
	glm::vec3 m_atlasValues = glm::vec3(1, 0, 0);

public:
	Sprite(Texture* texture, Color color, int numberOfRows, int frameCount, int frameOffset, float frameTime) : m_texture(texture), m_color(color), m_numberOfRows(numberOfRows), m_frameCount(frameCount), m_frameOffset(frameOffset), m_frameTime(frameTime) {}
	Sprite() {}
	Texture* m_texture = nullptr;
	Color m_color = Color::White();
	int m_numberOfRows = 1;
	int m_frameCount = 0;
	int m_frameOffset = 0;
	float m_frameTime = 0;

	virtual void Draw(HDRPipeline* pipeline, glm::vec2 origin, float x, float y, float w, float h, float rotation);
};