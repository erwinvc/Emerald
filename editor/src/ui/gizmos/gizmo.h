#pragma once
#include "graphics/shaders/shader.h"
#include "graphics/render/material.h"
//#include "graphics/render/material.h"

namespace emerald {
class Gizmo {
public:
	enum class Operation {
		TRANSLATE,
		ROTATE,
		SCALE
	};

	enum class Axis {
		X,
		Y,
		Z,
		XY,
		YZ,
		XZ
	};

private:
	struct GizmoVertex {
		glm::vec3 position;
		glm::vec3 color;
	};

	Operation currentOperation = Operation::TRANSLATE;
	Axis selectedAxis = Axis::X;
	bool isDragging = false;
	glm::vec3 startDragPos;
	glm::mat4 targetTransform;

	std::vector<GizmoVertex> vertices;
	unsigned int VAO, VBO;
	Ref<Shader> shader;
	Ref<Material> material;
public:
	void initialize();
	void setOperation(Operation op) {
		currentOperation = op;
		updateGeometry();
	}

	void updateGeometry();
	void render(const glm::mat4& projection, const glm::mat4& view, const glm::mat4& model);

private:
	void addArrow(const glm::vec3& start, const glm::vec3& end, const glm::vec3& color) {
		// Add line
		addLine(start, end, color);

		// Add arrow head
		glm::vec3 direction = glm::normalize(end - start);
		glm::vec3 right = glm::normalize(glm::cross(direction, glm::vec3(0, 1, 0)));
		glm::vec3 up = glm::normalize(glm::cross(right, direction));

		float arrowSize = 0.1f;
		glm::vec3 arrowBase = end - direction * arrowSize;

		vertices.push_back({ end, color });
		vertices.push_back({ arrowBase + right * arrowSize, color });
		vertices.push_back({ arrowBase - right * arrowSize, color });

		vertices.push_back({ end, color });
		vertices.push_back({ arrowBase + up * arrowSize, color });
		vertices.push_back({ arrowBase - up * arrowSize, color });
	}

	void addLine(const glm::vec3& start, const glm::vec3& end, const glm::vec3& color) {
		vertices.push_back({ start, color });
		vertices.push_back({ end, color });
	}

	void addCircle(const glm::vec3& center, const glm::vec3& normal, const glm::vec3& color) {
		const int segments = 32;
		glm::vec3 tangent = glm::normalize(glm::cross(normal, glm::vec3(0, 1, 0)));
		glm::vec3 bitangent = glm::cross(normal, tangent);

		for (int i = 0; i < segments; ++i) {
			float angle1 = (float)i / segments * glm::two_pi<float>();
			float angle2 = (float)(i + 1) / segments * glm::two_pi<float>();

			glm::vec3 point1 = center + (tangent * cos(angle1) + bitangent * sin(angle1));
			glm::vec3 point2 = center + (tangent * cos(angle2) + bitangent * sin(angle2));

			addLine(point1, point2, color);
		}
	}

	void addCube(const glm::vec3& center, float size, const glm::vec3& color) {
		float halfSize = size * 0.5f;
		std::vector<glm::vec3> corners = {
			center + glm::vec3(-halfSize, -halfSize, -halfSize),
			center + glm::vec3(halfSize, -halfSize, -halfSize),
			center + glm::vec3(halfSize, halfSize, -halfSize),
			center + glm::vec3(-halfSize, halfSize, -halfSize),
			center + glm::vec3(-halfSize, -halfSize, halfSize),
			center + glm::vec3(halfSize, -halfSize, halfSize),
			center + glm::vec3(halfSize, halfSize, halfSize),
			center + glm::vec3(-halfSize, halfSize, halfSize)
		};

		// Bottom face
		addLine(corners[0], corners[1], color);
		addLine(corners[1], corners[2], color);
		addLine(corners[2], corners[3], color);
		addLine(corners[3], corners[0], color);

		// Top face
		addLine(corners[4], corners[5], color);
		addLine(corners[5], corners[6], color);
		addLine(corners[6], corners[7], color);
		addLine(corners[7], corners[4], color);

		// Connecting edges
		addLine(corners[0], corners[4], color);
		addLine(corners[1], corners[5], color);
		addLine(corners[2], corners[6], color);
		addLine(corners[3], corners[7], color);
	}
};
}
