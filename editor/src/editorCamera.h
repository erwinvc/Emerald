#pragma once
#include "graphics/camera.h"

namespace emerald {
	class EditorCamera : public Camera {
	public:
		EditorCamera(float fov, float nearClip, float farClip)
			: Camera(fov, nearClip, farClip), movementSpeed(5.0f), rotationSpeed(0.1f) {
		}

		void update(Timestep ts);

	private:
		float movementSpeed;
		float rotationSpeed;
		double lastMouseX = 0.0, lastMouseY = 0.0;

		glm::vec3 getForwardDirection() const;
		glm::vec3 getRightDirection() const;
		glm::vec3 getUpDirection() const;
	};
}