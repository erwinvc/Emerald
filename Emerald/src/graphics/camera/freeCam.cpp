#include "stdafx.h"

void FreeCam::Update(const TimeStep& time) {
	float speed = movementSpeed * time.GetSeconds();
	if (KeyDown(LSHIFT)) speed *= 10;
	if (KeyDown(LALT)) speed /= 10;

	if (KeyDown('W')) Utils::setPositionInFrontOfCam(transform.m_position, this, speed);
	if (KeyDown('S')) Utils::setPositionInFrontOfCam(transform.m_position, this, -speed);
	if (KeyDown('D')) transform.m_position += glm::vec3(Math::Cos(transform.m_rotation.y), 0, Math::Sin(transform.m_rotation.y))  * speed;
	if (KeyDown('A')) transform.m_position -= glm::vec3(Math::Cos(transform.m_rotation.y), 0, Math::Sin(transform.m_rotation.y))  * speed;

	if (KeyDown('I')) transform.m_position.z += speed;
	if (KeyDown('K')) transform.m_position.z -= speed;
	if (KeyDown('J')) transform.m_position.x += speed;
	if (KeyDown('L')) transform.m_position.x -= speed;
	if (KeyDown('Q') || KeyDown(' ')) transform.m_position.y += speed;
	if (KeyDown('E') || KeyDown('C') /*|| KeyDown(LCTRL)*/) transform.m_position.y -= speed;

	if (ButtonDown(VK_MOUSE_RIGHT)) {
		transform.m_rotation.y -= GetMouse()->GetDelta().x * 0.005f;
		transform.m_rotation.x = Math::Clamp(transform.m_rotation.x - GetMouse()->GetDelta().y * 0.005f, -Math::HALF_PI, Math::HALF_PI);
	}

	UpdateViewMatrix();
}