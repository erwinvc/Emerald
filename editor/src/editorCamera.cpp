#include "eepch.h"
#include "editorCamera.h"
#include "GLFW/glfw3.h"

namespace emerald {
	void EditorCamera::update(Timestep ts) {
		GLFWwindow* window = glfwGetCurrentContext();
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
			m_position += getForwardDirection() * movementSpeed * ts.deltaTime();
		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
			m_position -= getForwardDirection() * movementSpeed * ts.deltaTime();
		}
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
			m_position -= getRightDirection() * movementSpeed * ts.deltaTime();
		}
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
			m_position += getRightDirection() * movementSpeed * ts.deltaTime();
		}
		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
			m_position -= getUpDirection() * movementSpeed * ts.deltaTime();
		}
		if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
			m_position += getUpDirection() * movementSpeed * ts.deltaTime();
		}

		double mouseX, mouseY;
		glfwGetCursorPos(window, &mouseX, &mouseY);

		float deltaX = static_cast<float>(mouseX - lastMouseX);
		float deltaY = static_cast<float>(mouseY - lastMouseY);

		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
			m_rotation.y += deltaX * rotationSpeed * ts.deltaTime();
			m_rotation.x += deltaY * rotationSpeed * ts.deltaTime();
		}

		lastMouseX = mouseX;
		lastMouseY = mouseY;

		updateView();
	}

	glm::vec3 EditorCamera::getForwardDirection() const {
		return glm::normalize(glm::vec3(glm::sin(m_rotation.y), 0.0f, glm::cos(m_rotation.y)));
	}

	glm::vec3 EditorCamera::getRightDirection() const {
		return glm::normalize(glm::vec3(glm::cos(m_rotation.y), 0.0f, -glm::sin(m_rotation.y)));
	}

	glm::vec3 EditorCamera::getUpDirection() const {
		return glm::vec3(0.0f, 1.0f, 0.0f);
	}
}