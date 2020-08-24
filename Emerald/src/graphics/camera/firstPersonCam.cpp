#include "stdafx.h"

struct {
	glm::vec3 dimensions{ 0.3f, 1.42f, 0.3f };
} box;

float FirstPersonCam::CollideX(glm::vec3& vel, const glm::vec3& offset) {
	glm::vec3 relativeVel = vel * 0.333333f;
	auto& position = transform.position;
	AABB player(transform.position.x - 0.3f, transform.position.y - 1.42f, transform.position.z - 0.3f, transform.position.x + 0.3f, transform.position.y + 0.38f, transform.position.z + 0.3f);
	player = player.Offset(offset.x, offset.y, offset.z);
	//AABB playerOffsetVel = player.AddCoord(relativeVel.x, relativeVel.y, relativeVel.z);
	vector<AABB>& aabbs = GetWorld()->GetAABBs(player.AddCoord(relativeVel.x, 0, 0));
	for (auto& aabb : aabbs) {
		if (aabb.CalculateXOffset(player, relativeVel.x)) vel.x = 0;
	}
	return relativeVel.x;
}

float FirstPersonCam::CollideY(glm::vec3& vel, const glm::vec3& offset) {
	glm::vec3 relativeVel = vel * 0.333333f;
	AABB player(transform.position.x - 0.3f, transform.position.y - 1.42f, transform.position.z - 0.3f, transform.position.x + 0.3f, transform.position.y + 0.38f, transform.position.z + 0.3f);
	player = player.Offset(offset.x, offset.y, offset.z);
	//AABB playerOffsetVel = player.AddCoord(relativeVel.x, relativeVel.y, relativeVel.z);
	vector<AABB>& aabbs = GetWorld()->GetAABBs(player.AddCoord(0, relativeVel.y, 0));

	for (auto& aabb : aabbs) {
		if (aabb.CalculateYOffset(player, relativeVel.y)) vel.y = 0;
	}

	return relativeVel.y;
}

float FirstPersonCam::CollideZ(glm::vec3& vel, const glm::vec3& offset) {
	glm::vec3 relativeVel = vel * 0.333333f;
	AABB player(transform.position.x - 0.3f, transform.position.y - 1.42f, transform.position.z - 0.3f, transform.position.x + 0.3f, transform.position.y + 0.38f, transform.position.z + 0.3f);
	player = player.Offset(offset.x, offset.y, offset.z);
	//AABB playerOffsetVel = player.AddCoord(relativeVel.x, relativeVel.y, relativeVel.z);
	vector<AABB>& aabbs = GetWorld()->GetAABBs(player.AddCoord(0, 0, relativeVel.z));
	for (auto& aabb : aabbs) {
		if (aabb.CalculateZOffset(player, relativeVel.z)) vel.z = 0;
	}
	return relativeVel.z;
}
void FirstPersonCam::ResetPositionToBB(const AABB& axisalignedbb) {
	transform.position.x = (axisalignedbb.minX + axisalignedbb.maxX) / 2.0f;
	transform.position.y = axisalignedbb.minY;
	transform.position.z = (axisalignedbb.minZ + axisalignedbb.maxZ) / 2.0f;
}
void FirstPersonCam::Update() {
	lastUpdateTransform = transform;

	m_FOV = Math::Lerp(m_FOV, m_targetFOV, 0.016f * 12.0f);
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

	m_isOnGround = false;

	bool isFalling = velocity.y < 0;
	glm::vec3 temporaryVelocity = velocity;
	float newVelY = CollideY(temporaryVelocity, glm::vec3(0.0f, 0.0f, 0.0f));
	float newVelX = CollideX(temporaryVelocity, glm::vec3(0.0f, 0.0f, 0.0f));
	float newVelZ = CollideZ(temporaryVelocity, glm::vec3(0.0f, 0.0f, 0.0f));
	if (isFalling && temporaryVelocity.y == 0) m_isOnGround = true;

	glm::vec3 stepVelocity = velocity;
	float newVelStepY = CollideY(stepVelocity, glm::vec3(0.0f, m_stepHeight, 0.0f));
	float newVelStepX = CollideX(stepVelocity, glm::vec3(0.0f, m_stepHeight, 0.0f));
	float newVelStepZ = CollideZ(stepVelocity, glm::vec3(0.0f, m_stepHeight, 0.0f));

	if (m_isOnGround && newVelStepX * newVelStepX + newVelStepZ * newVelStepZ > newVelX * newVelX + newVelZ * newVelZ) {

		//Hacky way to get height above step aabb.
		glm::vec3 testVel(0.0f, -m_stepHeight * 3.0f, 0.0f);
		float distanceFromStep = -CollideY(testVel, glm::vec3(velocity.x * 0.3333f, m_stepHeight, velocity.z * 0.3333f));
		transform.position += glm::vec3(newVelStepX, m_stepHeight - distanceFromStep, newVelStepZ);
		velocity = glm::vec3(stepVelocity.x, 0.0f, stepVelocity.z);
	} else {
		transform.position += glm::vec3(newVelX, newVelY, newVelZ);
		velocity = temporaryVelocity;
	}

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

void FirstPersonCam::DrawUpdate(float partialTicks) {
	if (GetClient()->IsMouseLocked() || ButtonDown(VK_MOUSE_LEFT)) {
		auto change = GetMouse()->GetDelta();
		transform.rotation.x -= glm::radians(change.y * 0.15f);
		transform.rotation.y -= glm::radians(change.x * 0.15f);
	}

	transform.rotation.x = Math::Clamp(transform.rotation.x, -Math::HALF_PI, Math::HALF_PI);
	if (transform.rotation.y < 0) transform.rotation.y = Math::TWO_PI;
	if (transform.rotation.y > Math::TWO_PI) transform.rotation.y = 0;
	UpdateViewMatrix(partialTicks);
}