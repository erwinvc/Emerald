#include "stdafx.h"

void FirstPersonCam::Update(const TimeStep& time) {
	//float speed = movementSpeed * time.GetSeconds();
	//if (KeyDown(LSHIFT)) speed *= 10;
	//if (KeyDown(LALT)) speed /= 10;
	//
	//if (KeyDown('W')) Utils::setPositionInFrontOfCam(m_position, this, speed);
	//if (KeyDown('S')) Utils::setPositionInFrontOfCam(m_position, this, -speed);
	//if (KeyDown('D')) m_position += Vector3(Math::Cos(m_rotation.yaw) * speed, 0, Math::Sin(m_rotation.yaw) * speed);
	//if (KeyDown('A')) m_position -= Vector3(Math::Cos(m_rotation.yaw) * speed, 0, Math::Sin(m_rotation.yaw) * speed);
	//
	//if (KeyDown('Q') || KeyDown(' ')) m_position.y += speed;
	//if (KeyDown('E') || KeyDown('C') /*|| KeyDown(LCTRL)*/) m_position.y -= speed;
	//
	//if (ButtonDown(VK_MOUSE_RIGHT)) {
	//	m_rotation.y -= GetMouse()->GetDelta().x * 0.005f;
	//	m_rotation.x = Math::Clamp(m_rotation.x - GetMouse()->GetDelta().y * 0.005f, -Math::HALF_PI, Math::HALF_PI);
	//}

	if (!ImGui::GetIO().WantCaptureKeyboard) {
		float speed = movementSpeed * time.GetSeconds();

		m_rotation.y -= GetMouse()->GetDelta().x * 0.005f;
		m_rotation.x = Math::Clamp(m_rotation.x - GetMouse()->GetDelta().y * 0.005f, -Math::HALF_PI, Math::HALF_PI);

		Vector3 newPos = m_position;
		if (KeyDown('W')) Utils::setPositionInFrontOfCam(newPos, this, speed);
		if (KeyDown('S')) Utils::setPositionInFrontOfCam(newPos, this, -speed);
		if (KeyDown('D')) newPos += Vector3(Math::Cos(m_rotation.yaw) * speed, 0, Math::Sin(m_rotation.yaw) * speed);
		if (KeyDown('A')) newPos -= Vector3(Math::Cos(m_rotation.yaw) * speed, 0, Math::Sin(m_rotation.yaw) * speed);

		int posX = newPos.x;
		int posY = newPos.z;
		bool canMove = true;
		for (int i = 0; i < 8; i++) {
			Vector2I dir = WangTile::m_positionOffsets[i];
			Tile* tile = GetWorld()->GetTile(posX + dir.x, posY + dir.y);
			if (tile) {
				if (Math::Within(newPos.x, (float)tile->m_position.x - 0.5f, (float)tile->m_position.x + 0.5f)) {
					if (Math::Within(newPos.z, (float)tile->m_position.y - 0.5f, (float)tile->m_position.y + 0.5f))
						canMove = false;
				}
			}
		}
		if (canMove) m_position = newPos;
		m_position.y = cameraHeight;
	}

	UpdateViewMatrix();
}