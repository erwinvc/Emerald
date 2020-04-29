#include "stdafx.h"

glm::vec3 GroundRaycast::GetMousePosition(Camera* camera) {
	return CalculateMouseRay(camera ? camera : Camera::active);
}

glm::vec3 GroundRaycast::GetScreenPosition(glm::vec2 position, Camera* camera) {
	position.x = (position.x * 2.0f) - 1.0f;
	position.y = -1.0f * ((position.y * 2.0f) - 1.0f);
	return CalculateScreenRay(glm::vec2(position), camera ? camera : Camera::active);
}

glm::vec3 GroundRaycast::CalculateScreenRay(const glm::vec2& position, Camera* camera) {
	glm::vec4& clipCoords = glm::vec4(position.x, position.y, -1.0f, -1.0f);
	glm::vec4& eyeCoords = ToEyeCoords(clipCoords, camera);
	glm::vec3& worldRay = ToWorldCoords(eyeCoords, camera);
	return worldRay;
}
glm::vec3 GroundRaycast::CalculateMouseRay(Camera* camera) {
	float mouseX = GetMouse()->GetPosition().x;
	float mouseY = GetMouse()->GetPosition().y;
	glm::vec4& viewport = camera->GetViewport();
	mouseX = Math::Clamp(mouseX, viewport.x, viewport.x + viewport.z);
	mouseY = Math::Clamp(mouseY, viewport.y, viewport.y + viewport.w);
	mouseX = Math::Map(mouseX, viewport.x, viewport.x + viewport.z, -1.0f, 1.0f);
	mouseY = Math::Map(mouseY, viewport.y, viewport.y + viewport.w, 1.0f, -1.0f);
	//LOG("%f, %f", mouseX, mouseY);
	glm::vec4& clipCoords = glm::vec4(mouseX, mouseY, -1.0f, -1.0f);
	glm::vec4& eyeCoords = ToEyeCoords(clipCoords, camera);
	glm::vec3& worldRay = ToWorldCoords(eyeCoords, camera);
	return worldRay;
}

glm::vec4 GroundRaycast::ToEyeCoords(const glm::vec4& clipCoords, Camera* camera) {
	glm::mat4& invertedProjection = camera->GetInverseProjectionMatrix();
	glm::vec4& eyeCoords = invertedProjection * clipCoords;
	return glm::vec4{ eyeCoords.x, eyeCoords.y, -1.0f, 0.0f };
}
glm::vec3 GroundRaycast::ToWorldCoords(const glm::vec4& eyeCoords, Camera* camera) {
	glm::mat4& invertedView = camera->GetInverseViewMatrix();
	glm::vec4& rayWorld = invertedView * eyeCoords;
	glm::vec3& mouseRay = glm::vec3(rayWorld.x, rayWorld.y, rayWorld.z);
	return glm::normalize(mouseRay);
}
glm::vec3 GroundRaycast::GetGroundPosition(const glm::vec3& ray, float height, Camera* camera) {
	glm::vec3& rayStart = camera->transform.m_position;
	glm::vec3& rayEnd = camera->transform.m_position + ray;
	glm::vec3& delta = rayEnd - rayStart;
	delta = glm::normalize(delta);

	glm::vec3 rayToPlaneDelta = glm::vec3(0.0f, height, 0.0f) - rayStart;

	float ratio = glm::dot(rayToPlaneDelta, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::vec3& proj = glm::vec3(0.0f, 1.0f, 0.0f) * ratio;

	auto wp = glm::dot(rayToPlaneDelta, glm::vec3(0.0f, 1.0f, 0.0f));
	auto vp = glm::dot(delta, glm::vec3(0.0f, 1.0f, 0.0f));
	auto k = wp / vp;
	//return delta.Multiply(k).Add(rayStart);
	return delta * k + rayStart;
}

glm::ivec2 GroundRaycast::GetTile() {
	//glm::vec3& ray = GetMousePosition();
	//glm::vec3& pos = GetGroundPosition(ray);
	//return { (int)pos.x, (int)pos.z };
	return glm::ivec2();
}
