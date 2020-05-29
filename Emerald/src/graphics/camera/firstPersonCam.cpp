#include "stdafx.h"

struct {
	glm::vec3 dimensions{ 0.3f, 1.62f, 0.3f };
} box;

float RoughRound(float value) {
	return ((float)(Math::Round(value * 100))) / 100;
}

void FirstPersonCam::Collide(const glm::vec3& vel) {
	auto& position = transform.position;
	for (float x = Math::Floor(position.x - box.dimensions.x); x < position.x + box.dimensions.x; x++) {
		for (float y = Math::Floor(position.y - box.dimensions.y); y < position.y + 0.28f; y++) {
			for (float z = Math::Floor(position.z - box.dimensions.z); z < position.z + box.dimensions.z; z++) {
				auto block = GetWorld()->GetBlock(glm::vec3{ x, y, z });
				if (block != 0) {
					if (vel.y > 0) {
						position.y = y - 0.28f - 0.002f;
						m_velocity.y = 0;
					} else if (vel.y < 0) {
						m_isOnGround = true;
						position.y = y + box.dimensions.y + 1.001f;
						m_velocity.y = 0;
					}

					if (vel.x > 0) {
						position.x = x - box.dimensions.x - 0.001f;
					} else if (vel.x < 0) {
						position.x = x + box.dimensions.x + 1.001f;
					}

					if (vel.z > 0) {
						position.z = z - box.dimensions.z - 0.001f;
					} else if (vel.z < 0) {
						position.z = z + box.dimensions.z + 1.001f;
					}
				}
			}
		}
	}
	//LOG("%f", position.y);
}

static float FOV = 90;
static float goalFov = 90;
static int i = 0;
static float delta;
void FirstPersonCam::Update(const TimeStep& time) {
	FOV = Math::Lerp(FOV, goalFov, time * 12);
	i = 0;
	delta = 1.0f / time.DeltaTime();
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
		m_velocity.y += 0.48f;
	}

	if (moveForward > 0 && KeyDown(LSHIFT)) {
		moveForward *= 2.70f;
		goalFov = 90 * 1.1f;
	} else goalFov = 90;

	SetProjectionMatrix(FOV, m_nearPlane, m_farPlane);
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
		m_velocity += glm::vec3(xComponent, 0.0f, zComponent);
	}

	if (KeyJustDown('N')) m_grabMouse ^= true;
	if (m_grabMouse || ButtonDown(VK_MOUSE_LEFT)) {
		auto change = GetMouse()->GetDelta();
		transform.rotation.x -= glm::radians(change.y * 0.15f);
		transform.rotation.y -= glm::radians(change.x * 0.15f);
		//transform.rotation.x += (float)(change.y / 8.0f * -0.035f);
		//transform.rotation.y += (float)(change.x / 8.0f * -0.035f);
	}

	//Limit view angle
	transform.rotation.x = Math::Clamp(transform.rotation.x, -Math::HALF_PI, Math::HALF_PI);

	m_isOnGround = false;


	transform.position.x += m_velocity.x * 0.333333f;
	Collide({ m_velocity.x, 0, 0 });

	transform.position.y += m_velocity.y * 0.333333f;
	Collide({ 0, m_velocity.y, 0 });

	transform.position.z += m_velocity.z * 0.333333f;
	Collide({ 0, 0, m_velocity.z });

	if (transform.position.x < -32) {
		transform.position.x = -32;
		if (m_velocity.x < 0) m_velocity.x = 0;
	} else if (transform.position.x > 32) {
		transform.position.x = 32;
		if (m_velocity.x > 0) m_velocity.x = 0;
	}
	if (transform.position.z < -32) {
		transform.position.z = -32;
		if (m_velocity.z < 0) m_velocity.z = 0;
	} else if (transform.position.z > 32) {
		transform.position.z = 32;
		if (m_velocity.z > 0) m_velocity.z = 0;
	}
	m_velocity.x *= slipperiness;
	m_velocity.z *= slipperiness;
	m_velocity.y -= 0.02666666666f;
	m_velocity.y *= 0.99328838838f;
	UpdateViewMatrix();
}

void FirstPersonCam::DrawUpdate() {
	//transform.rotation.x = Math::Lerp(transform.rotation.x, m_targetRotation.x, i * (1.0f / delta));
	//transform.rotation.y = Math::Lerp(transform.rotation.y, m_targetRotation.y, i * (1.0f / delta));
	//transform.rotation.z = Math::Lerp(transform.rotation.z, m_targetRotation.z, i++ * (1.0f / delta));
}
