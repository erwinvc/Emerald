#include "eepch.h"
#include "gizmo.h"
#include "graphics/core/renderer.h"
#include "graphics/buffers/vertexArray.h"
#include "graphics/render/material.h"
#include "graphics/shaders/shaderRegistry.h"

namespace emerald {
	static const VertexBufferLayout s_layout = {
	{VertexAttributeType::FLOAT3, "vsPos", 0},
	{VertexAttributeType::FLOAT3, "vsColor", 0}
	};


	void Gizmo::initialize() {
		vbo = Ref<VertexBuffer>::create(BufferUsage::DYNAMIC);
		vao = Ref<VertexArray>::create("DE TEST VAO!", s_layout);
		vao->addBuffer(vbo);

		shader = ShaderRegistry::getShader("Gizmo");

		material = Ref<Material>::create("Gizmo", shader);
	}

	void Gizmo::updateGeometry() {
		vertices.clear();

		// Colors for each axis
		const glm::vec3 xColor(1.0f, 0.0f, 0.0f);
		const glm::vec3 yColor(0.0f, 1.0f, 0.0f);
		const glm::vec3 zColor(0.0f, 0.0f, 1.0f);

		float axisLength = 1.0f;
		float arrowSize = 0.1f;

		switch (currentOperation) {
			case Operation::TRANSLATE:
				// X axis
				addArrow(glm::vec3(0), glm::vec3(axisLength, 0, 0), xColor);
				// Y axis
				addArrow(glm::vec3(0), glm::vec3(0, axisLength, 0), yColor);
				// Z axis
				addArrow(glm::vec3(0), glm::vec3(0, 0, axisLength), zColor);
				break;

			case Operation::ROTATE:
				// Add rotation circles
				addCircle(glm::vec3(0), glm::vec3(1, 0, 0), xColor);
				addCircle(glm::vec3(0), glm::vec3(0, 1, 0), yColor);
				addCircle(glm::vec3(0), glm::vec3(0, 0, 1), zColor);
				break;

			case Operation::SCALE:
				// Add scale cubes at the end of each axis
				addCube(glm::vec3(axisLength, 0, 0), 0.1f, xColor);
				addCube(glm::vec3(0, axisLength, 0), 0.1f, yColor);
				addCube(glm::vec3(0, 0, axisLength), 0.1f, zColor);
				// Add lines
				addLine(glm::vec3(0), glm::vec3(axisLength, 0, 0), xColor);
				addLine(glm::vec3(0), glm::vec3(0, axisLength, 0), yColor);
				addLine(glm::vec3(0), glm::vec3(0, 0, axisLength), zColor);
				break;
		}

		// Update GPU buffer
		//glBindBuffer(GL_ARRAY_BUFFER, VBO);
		//glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GizmoVertex), vertices.data(), GL_STATIC_DRAW);

		vbo->setData((byte*)vertices.data(), (uint32_t)(vertices.size() * sizeof(GizmoVertex)));
		vao->validate();

	}
	void Gizmo::render(const glm::mat4& projection, const glm::mat4& view, const glm::mat4& model) {
		shader->bind();
		vao->bind();

		material->set("uProjection", projection);
		material->set("uView", view);
		material->set("uModel", model);

		material->updateForRendering();


		// Enable line smoothing for better looking gizmos
		glEnable(GL_LINE_SMOOTH);
		glLineWidth(10.0f);  // Thicker lines for better visibility

		// Disable depth testing to always draw gizmos on top
		glDepthFunc(GL_ALWAYS);

		// Draw lines
		glDrawArrays(GL_LINES, 0, (uint32_t)vertices.size());
		//Log::info("Drawing gizmo with {} vertices", vertices.size());
		// Draw arrow heads and other triangles
		// You might want to keep track of where triangle vertices start in your vertex buffer
		// glDrawArrays(GL_TRIANGLES, triangleStartIndex, triangleCount);

		// Restore depth testing
		glDepthFunc(GL_LESS);

		// Reset line width
		glLineWidth(1.0f);
	}
}