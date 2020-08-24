#include "stdafx.h"

void FreeCam::Update() {
	lastUpdateTransform = transform;
	float speed = movementSpeed * 0.016f;
	if (KeyDown(LSHIFT)) speed *= 10;
	if (KeyDown(LALT)) speed /= 10;

	if (KeyDown('W')) Utils::SetPositionInFrontOfCam(transform.position, this, speed);
	if (KeyDown('S')) Utils::SetPositionInFrontOfCam(transform.position, this, -speed);
	if (KeyDown('D')) transform.position += glm::vec3(Math::Cos(transform.rotation.y), 0, Math::Sin(transform.rotation.y))  * speed;
	if (KeyDown('A')) transform.position -= glm::vec3(Math::Cos(transform.rotation.y), 0, Math::Sin(transform.rotation.y))  * speed;

	if (KeyDown('I')) transform.position.z += speed;
	if (KeyDown('K')) transform.position.z -= speed;
	if (KeyDown('J')) transform.position.x += speed;
	if (KeyDown('L')) transform.position.x -= speed;
	if (KeyDown('Q') || KeyDown(' ')) transform.position.y += speed;
	if (KeyDown('E') || KeyDown('C') /*|| KeyDown(LCTRL)*/) transform.position.y -= speed;

	//if (ButtonDown(VK_MOUSE_RIGHT)) {
	//	transform.rotation.y -= GetMouse()->GetDelta().x * 0.005f;
	//	transform.rotation.x = Math::Clamp(transform.rotation.x - GetMouse()->GetDelta().y * 0.005f, -Math::HALF_PI, Math::HALF_PI);
	//}

	if (GetClient()->IsMouseLocked() || ButtonDown(VK_MOUSE_LEFT)) {
		auto change = GetMouse()->GetDelta();
		transform.rotation.x -= glm::radians(change.y * 0.15f);
		transform.rotation.y -= glm::radians(change.x * 0.15f);
	}
}