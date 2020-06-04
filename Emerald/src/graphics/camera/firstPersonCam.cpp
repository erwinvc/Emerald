#include "stdafx.h"

struct {
	glm::vec3 dimensions{ 0.3f, 1.42f, 0.3f };
} box;

float RoughRound(float value) {
	return ((float)(Math::Round(value * 100))) / 100;
}

void FirstPersonCam::Collide(const glm::vec3& vel) {
	auto& position = transform.position;
	for (float x = Math::Floor(position.x - box.dimensions.x); x < position.x + box.dimensions.x; x++) {
		for (float y = Math::Floor(position.y - box.dimensions.y); y < position.y + 0.38f; y++) {
			for (float z = Math::Floor(position.z - box.dimensions.z); z < position.z + box.dimensions.z; z++) {
				auto block = GetWorld()->GetBlock(glm::vec3{ x, y, z });
				if (block != 0) {
					if (vel.y > 0) {
						float diff = position.y - (y - 0.38f - 0.002f);
						if (diff < 0.5f)
							position.y = y - 0.38f - 0.002f;
						velocity.y = 0;
					} else if (vel.y < 0) {
						m_isOnGround = true;

						float diff = position.y - (y + box.dimensions.y + 1.001f);
						if (diff > -0.5f)
							position.y = y + box.dimensions.y + 1.001f;
						velocity.y = 0;
					}

					if (vel.x > 0) {
						float diff = position.x - (x - box.dimensions.x - 0.001f);
						if (diff < 0.5f)
							position.x = x - box.dimensions.x - 0.001f;
					} else if (vel.x < 0) {
						float diff = position.x - (x + box.dimensions.x + 1.001f);
						if (diff > -0.5f)
							position.x = x + box.dimensions.x + 1.001f;
					}

					if (vel.z > 0) {
						float diff = position.z - (z - box.dimensions.z - 0.001f);
						if (diff < 0.5f)
							position.z = z - box.dimensions.z - 0.001f;
					} else if (vel.z < 0) {
						float diff = position.z - (z + box.dimensions.z + 1.001f);
						if (diff > -0.5f)
							position.z = z + box.dimensions.z + 1.001f;
					}
				}
			}
		}
	}
}

void FirstPersonCam::Update() {
	lastUpdateTransform = transform;

	m_FOV = Math::Lerp(m_FOV, m_targetFOV, 0.016f * 12);
	float moveForward = 0.0f;
	float moveStrafe = 0.0f;

	if (KeyDown('W')) {
		moveForward += 1.0f;
	}
	if (KeyDown('S')) {
		moveForward -= 1.0f;
	}
	if (KeyDown('A')) {
		moveStrafe -= 1.0f;
	}
	if (KeyDown('D')) {
		moveStrafe += 1.0f;
	}
	if (KeyDown(' ') && m_isOnGround) {
		velocity.y += 0.48f;
	}

	if (moveForward > 0 && KeyDown(LSHIFT)) {
		moveForward *= 2.70f;
		m_targetFOV = 90 * 1.1f;
	} else m_targetFOV = 90;

	SetProjectionMatrix(m_FOV, m_nearPlane, m_farPlane);
	float multiplier;
	float slipperiness;
	float jumpMovementFactor = 0.02f;
	float moveSpeed = 0.1f;
	float max = 1.0f;

	if (KeyDown(LSHIFT)) {
		jumpMovementFactor = jumpMovementFactor + 0.02f * 0.3f;
		moveSpeed = 0.13f;
	}
	if (m_isOnGround) {
		slipperiness = 0.6f * 0.91f;
		multiplier = moveSpeed * 0.16277136F / (slipperiness * slipperiness * slipperiness);
	} else {
		slipperiness = 0.91f;
		multiplier = jumpMovementFactor;
	}

	float speed = Math::Sqrt(moveStrafe * moveStrafe + moveForward * moveForward);
	if (speed > 0.01f) {

		speed = multiplier / Math::Max(speed, max);

		moveStrafe *= speed;
		moveForward *= speed;

		float yawXComponent = glm::cos(transform.rotation.y - Math::HALF_PI);
		float yawYComponent = glm::sin(transform.rotation.y - Math::HALF_PI);

		float xComponent = moveForward * yawXComponent - moveStrafe * yawYComponent;
		float zComponent = moveStrafe * yawXComponent + moveForward * yawYComponent;
		velocity += glm::vec3(xComponent, 0.0f, zComponent);
	}

	if (GetClient()->m_lockedMouse || ButtonDown(VK_MOUSE_LEFT)) {
		auto change = GetMouse()->GetDelta();
		transform.rotation.x -= glm::radians(change.y * 0.15f);
		transform.rotation.y -= glm::radians(change.x * 0.15f);
	}

	//Limit view angle
	transform.rotation.x = Math::Clamp(transform.rotation.x, -Math::HALF_PI, Math::HALF_PI);

	m_isOnGround = false;

	//LOG("%f %f %f", velocity.x, velocity.y, velocity.z);

	transform.position.x += velocity.x * 0.333333f;
	Collide({ velocity.x, 0, 0 });

	transform.position.y += velocity.y * 0.333333f;
	Collide({ 0, velocity.y, 0 });

	transform.position.z += velocity.z * 0.333333f;
	Collide({ 0, 0, velocity.z });

	if (transform.position.x < -31.5f) {
		transform.position.x = -31.5f;
		if (velocity.x < 0) velocity.x = 0;
	} else if (transform.position.x > 31.5f) {
		transform.position.x = 31.5f;
		if (velocity.x > 0) velocity.x = 0;
	}
	if (transform.position.z < -31.5f) {
		transform.position.z = -31.5f;
		if (velocity.z < 0) velocity.z = 0;
	} else if (transform.position.z > 31.5f) {
		transform.position.z = 31.5f;
		if (velocity.z > 0) velocity.z = 0;
	}
	velocity.x *= slipperiness;
	velocity.z *= slipperiness;
	velocity.y -= 0.02666666666f;
	velocity.y *= 0.99328838838f;
}

void FirstPersonCam::OnImGui() {
	ImGui::InputFloat3("Position", (float*)&transform.position);
	ImGui::InputFloat3("Rotation", (float*)&transform.rotation);
	if (ImGui::InputFloat("Near", &m_nearPlane, 0.000001f, 10000.0f)) {
		UpdateProjectionMatrix();
	}

	if (ImGui::InputFloat("Far", &m_farPlane, 0.000001f, 10000.0f)) {
		UpdateProjectionMatrix();
	}

	if (ImGui::SliderFloat("FOV", &m_FOV, 0, 180)) {
		UpdateProjectionMatrix();
	}
}