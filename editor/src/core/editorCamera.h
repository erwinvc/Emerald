#pragma once

#include "graphics/core/camera.h"
namespace emerald {
	class EditorCamera : public Camera {
	public:
		EditorCamera(float fov, float nearClip, float farClip)
			: Camera(fov, nearClip, farClip), m_movementSpeed(5.0f), m_rotationSpeed(0.005f) {
		}

		void update(Timestep ts);
	private:
		float m_movementSpeed;
		float m_rotationSpeed;
	};
}