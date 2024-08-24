#include "eepch.h"
#include "editorCamera.h"
#include "input/keyboard.h"
#include "input/mouse.h"
#include "glm/ext/scalar_common.hpp"
#include "glm/gtc/constants.hpp"

namespace emerald {
	void EditorCamera::update(Timestep ts) {
		float deltaSpeed = m_movementSpeed * ts.deltaTime();

		if (Keyboard::keyDown(Key::LEFT_SHIFT)) deltaSpeed *= 10;
		if (Keyboard::keyDown(Key::LEFT_ALT)) deltaSpeed /= 10;

		if (Keyboard::keyDown(Key::Q) || Keyboard::keyDown(Key::SPACE)) m_position.y += deltaSpeed;
		if (Keyboard::keyDown(Key::E) || Keyboard::keyDown(Key::C) /*|| KeyDown(LCTRL)*/) m_position.y -= deltaSpeed;

		if (Keyboard::keyDown(Key::D)) m_position += right() * deltaSpeed;
		if (Keyboard::keyDown(Key::A)) m_position -= right() * deltaSpeed;

		if (Keyboard::keyDown(Key::W)) m_position += forward() * deltaSpeed;
		if (Keyboard::keyDown(Key::S)) m_position -= forward() * deltaSpeed;

		if (Mouse::buttonDown(MouseButton::RIGHT)) {
			m_rotation.y += Mouse::positionDelta().x * m_rotationSpeed;
			m_rotation.x = glm::clamp((float)(m_rotation.x + Mouse::positionDelta().y * m_rotationSpeed), -glm::half_pi<float>(), glm::half_pi<float>());
		}

		updateViewMatrix();
	}
}