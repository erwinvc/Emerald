#pragma once

class Camera;

class GroundRaycast {
private:
	Vector3 Get(Camera& camera);
	Vector3 CalculateMouseRay(Camera& camera);
	Vector2 GetNormalizedDeviceCoords(float mouseX, float mouseY);
	Vector4 ToEyeCoords(Vector4& clipCoords, Camera& camera);
	Vector3 ToWorldCoords(Vector4& eyeCoords, Camera& camera);
};