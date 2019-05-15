#pragma once

class FreeCam : public Camera {
private:
public:
	FreeCam(float fov, float aspectRatio, float nearPlane, float farPlane) : Camera(fov, aspectRatio, nearPlane, farPlane) {}

    const float movementSpeed = 10;

	void FixedUpdate() {}
    void Update(const TimeStep& time) {
        float speed = movementSpeed * time.GetSeconds();
        if (KeyDown(LSHIFT)) speed *= 10;
        if (KeyDown(LALT)) speed /= 10;

        if (KeyDown('W')) Utils::setPositionInFrontOfCam(m_position, *this, speed);
        if (KeyDown('S')) Utils::setPositionInFrontOfCam(m_position, *this ,-speed);
        if (KeyDown('D')) m_position += Vector3(Math::Cos(m_rotation.y) * speed, 0, Math::Sin(m_rotation.y) * speed);
        if (KeyDown('A')) m_position -= Vector3(Math::Cos(m_rotation.y) * speed, 0, Math::Sin(m_rotation.y) * speed);
            
        if (KeyDown('Q') || KeyDown(' ')) m_position.y += speed;
        if (KeyDown('E') || KeyDown('C') || KeyDown(LCTRL)) m_position.y -= speed;

        if (ButtonDown(VK_MOUSE_LEFT) || ButtonDown(VK_MOUSE_RIGHT)) {
            m_rotation.y -= GetMouse()->GetDelta().x * 0.005f;
            m_rotation.x = Math::Clamp(m_rotation.x - GetMouse()->GetDelta().y * 0.005f, -Math::HALF_PI, Math::HALF_PI);
        }

		UpdateViewMatrix();
    }

};