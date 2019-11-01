#include "stdafx.h"

Vector3 GroundRaycast::GetMousePosition() {
	return CalculateMouseRay();
}

Vector3 GroundRaycast::GetScreenPosition(Vector2 position) {
	position.x = (position.x * 2.0f) - 1.0f;
	position.y = -1.0f * ((position.y * 2.0f) - 1.0f);
	return CalculateScreenRay(Vector2(position));
}

Vector3 GroundRaycast::CalculateScreenRay(const Vector2& position) {
	Camera* camera = GetCamera();
	Vector4& clipCoords = Vector4(position.x, position.y, -1.0f, -1.0f);
	Vector4& eyeCoords = ToEyeCoords(clipCoords, camera);
	Vector3& worldRay = ToWorldCoords(eyeCoords, camera);
	return worldRay;
}
Vector3 GroundRaycast::CalculateMouseRay() {
	Camera* camera = GetCamera();
	float mouseX = GetMouse()->GetPosition().x;
	float mouseY = GetMouse()->GetPosition().y;
	Vector4& viewport = GetCamera()->GetViewport();
	mouseX = Math::Clamp(mouseX, viewport.x, viewport.x + viewport.z);
	mouseY = Math::Clamp(mouseY, viewport.y, viewport.y + viewport.w);
	mouseX = Math::Map(mouseX, viewport.x, viewport.x + viewport.z, -1.0f, 1.0f);
	mouseY = Math::Map(mouseY, viewport.y, viewport.y + viewport.w, 1.0f, -1.0f);
	//LOG("%f, %f", mouseX, mouseY);
	Vector4& clipCoords = Vector4(mouseX, mouseY, -1.0f, -1.0f);
	Vector4& eyeCoords = ToEyeCoords(clipCoords, camera);
	Vector3& worldRay = ToWorldCoords(eyeCoords, camera);
	return worldRay;
}

Vector4 GroundRaycast::ToEyeCoords(const Vector4& clipCoords, const AssetRef<Camera>& camera) {
	Matrix4& invertedProjection = Matrix4::Invert(camera->GetProjectionMatrix());
	Vector4& eyeCoords = invertedProjection * clipCoords;
	return Vector4{ eyeCoords.x, eyeCoords.y, -1.0f, 0.0f };
}
Vector3 GroundRaycast::ToWorldCoords(const Vector4& eyeCoords, const AssetRef<Camera>& camera) {
	Matrix4& invertedView = Matrix4::Invert(camera->GetViewMatrix());
	Vector4& rayWorld = invertedView * eyeCoords;
	Vector3& mouseRay = Vector3(rayWorld.x, rayWorld.y, rayWorld.z);
	return mouseRay.Normalize();
}
Vector3 GroundRaycast::GetGroundPosition(const Vector3& ray, float height) {
	Vector3& rayStart = GetCamera()->m_position;
	Vector3& rayEnd = GetCamera()->m_position + ray;
	Vector3& delta = rayEnd.Subtract(rayStart);
	delta.Normalize();

	Vector3 rayToPlaneDelta = Vector3(0.0f, height, 0.0f).Subtract(rayStart);

	float ratio = rayToPlaneDelta.Dot(Vector3::Up());
	Vector3& proj = Vector3::Up().Multiply(ratio);

	auto wp = rayToPlaneDelta.Dot(Vector3::Up());
	auto vp = delta.Dot(Vector3::Up());
	auto k = wp / vp;
	return delta.Multiply(k).Add(rayStart);
}

Vector2I GroundRaycast::GetTile() {
	//Vector3& ray = GetMousePosition();
	//Vector3& pos = GetGroundPosition(ray);
	//return { (int)pos.x, (int)pos.z };
	return Vector2I();
}
