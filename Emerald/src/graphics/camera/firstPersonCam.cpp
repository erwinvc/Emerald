#include "stdafx.h"

struct {
	glm::vec3 dimensions{ 0.3f, 1.62f, 0.3f };
} box;

void FirstPersonCam::Collide(const glm::vec3& vel) {
	auto& position = transform.position;
	for (int x = (int)(position.x - box.dimensions.x); x < position.x + box.dimensions.x; x++) {
		for (int y = (int)(position.y - box.dimensions.y); y < position.y + 0.28f; y++) {
			for (int z = (int)(position.z - box.dimensions.z); z < position.z + box.dimensions.z; z++) {
				auto block = GetWorld()->GetBlock(glm::vec3{ x, y, z });
				if (block != 0) {
					if (vel.y > 0) {
						//LOG("Set %f %f", position.y, y - box.dimensions.x);
						position.y = y - box.dimensions.x;
						m_velocity.y = 0;
					} else if (vel.y < 0) {
						m_isOnGround = true;
						position.y = y + box.dimensions.y + 1;
						m_velocity.y = 0;
					}
					if (vel.x > 0) {
						position.x = x - box.dimensions.x;
					} else if (vel.x < 0) {
						position.x = x + box.dimensions.x + 1;
					}
					if (vel.z > 0) {
						position.z = z - box.dimensions.z;
					} else if (vel.z < 0) {
						position.z = z + box.dimensions.z + 1;
					}
				}
			}
		}
	}
	//LOG("%f", position.y);
}

static int i = 0;
static float delta;
void FirstPersonCam::Update(const TimeStep& time) {
	i = 0;
	delta = 1.0f / time.DeltaTime();
	float moveForward = 0.0f;
	float moveStrafe = 0.0f;

	if (KeyDown('W')) {
		moveForward += 1.0f;
	} else if (KeyDown('S')) {
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
	}

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
	Collide({ m_velocity.x * 0.333333f, 0, 0 });

	transform.position.y += m_velocity.y * 0.333333f;
	Collide({ 0, m_velocity.y * 0.333333f, 0 });

	transform.position.z += m_velocity.z * 0.333333f;
	Collide({ 0, 0, m_velocity.z * 0.333333f });


	m_velocity.x *= slipperiness;
	m_velocity.z *= slipperiness;
	m_velocity.y -= 0.02666666666f;
	m_velocity.y *= 0.99328838838f;

	//float r = 0.000001f;
	//glm::vec3 velChange = m_velocity * (Math::Pow(r, time.DeltaTime()) - 1.0f) / Math::Log(r);
	//
	//transform.position.x += velChange.x;
	//Collide({ velChange.x, 0, 0 });
	//
	//transform.position.y += velChange.y;
	//Collide({ 0, velChange.y * time, 0 });
	//
	//transform.position.z += velChange.z;
	//Collide({ 0, 0, velChange.z * time });
	//
	//float PLAYER_SPEED = 1.6f;
	//float PLAYER_JUMP = 0.4f;
	//if (KeyDown(LSHIFT)) {
	//	PLAYER_SPEED *= 2.5f;
	//}
	//
	//if (!m_isOnGround) {
	//	PLAYER_SPEED *= 0.1f;
	//}
	//
	//if (KeyDown('W')) {
	//	m_acceleration.x += glm::cos(transform.rotation.y - Math::HALF_PI) * PLAYER_SPEED;
	//	m_acceleration.z += glm::sin(transform.rotation.y - Math::HALF_PI) * PLAYER_SPEED;
	//} else if (KeyDown('S')) {
	//	m_acceleration.x -= glm::cos(transform.rotation.y - Math::HALF_PI) * PLAYER_SPEED;
	//	m_acceleration.z -= glm::sin(transform.rotation.y - Math::HALF_PI) * PLAYER_SPEED;
	//}
	//if (KeyDown('A')) {
	//	m_acceleration -= transform.Right() * PLAYER_SPEED;
	//} else if (KeyDown('D')) {
	//	m_acceleration += transform.Right() * PLAYER_SPEED;
	//}
	//
	//if (KeyDown(' ') && m_isOnGround) {
	//	m_acceleration.y += PLAYER_JUMP * 20;
	//}
	//
	//m_velocity += m_acceleration;
	//m_acceleration = { 0, 0, 0 };
	//
	////Movement damping if not in air
	//if (m_isOnGround) {
	//	float damping = Math::Pow(r, time);
	//	m_velocity.x *= damping;
	//	m_velocity.z *= damping;
	//} else {
	//	float damping = Math::Pow(r2, time);
	//	m_velocity.x *= damping;
	//	m_velocity.z *= damping;
	//	
	////Falling
	//	m_velocity.y -= 20 * time;
	//}
	//
	////Mouse rotation
	//if (KeyJustDown('N')) m_grabMouse ^= true;
	//if (m_grabMouse || ButtonDown(VK_MOUSE_LEFT)) {
	//	auto change = GetMouse()->GetDelta();
	//	transform.rotation.x += (float)(change.y / 8.0f * -0.035f);
	//	transform.rotation.y += (float)(change.x / 8.0f * -0.035f);
	//}
	//
	////Limit view angle
	//transform.rotation.x = Math::Clamp(transform.rotation.x, -Math::HALF_PI, Math::HALF_PI);

	UpdateViewMatrix();
}

void FirstPersonCam::DrawUpdate() {
	//transform.rotation.x = Math::Lerp(transform.rotation.x, m_targetRotation.x, i * (1.0f / delta));
	//transform.rotation.y = Math::Lerp(transform.rotation.y, m_targetRotation.y, i * (1.0f / delta));
	//transform.rotation.z = Math::Lerp(transform.rotation.z, m_targetRotation.z, i++ * (1.0f / delta));
}
