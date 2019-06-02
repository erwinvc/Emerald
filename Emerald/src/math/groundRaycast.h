#pragma once

class Camera;

class GroundRaycast {
public:
	Vector3& Get(Camera* camera);

	Vector3& GetGroundPosition();

private:
	Vector3& CalculateMouseRay(Camera* camera);
	Vector2& GetNormalizedDeviceCoords(float mouseX, float mouseY);
	Vector4& ToEyeCoords(Vector4& clipCoords, Camera* camera);
	Vector3& ToWorldCoords(Vector4& eyeCoords, Camera* camera);

	//Tile search(Vector3 ray, float distance, Tile[][] objects) {
	//	if (distance > RAY_RANGE) return null;
	//	Vector3F point = getPointOnRay(ray, distance);
	//	for (Tile[] t : objects) {
	//		for (Tile tile : t) {
	//			if (within(point, tile)) return tile;
	//		}
	//	}
	//	distance += 0.1f;
	//	return search(ray, distance, objects);
	//}
	//
	//Tile search(Vector3F ray, float distance, ArrayList<Tile> objects) {
	//	if (distance > RAY_RANGE) return null;
	//	Vector3F point = getPointOnRay(ray, distance);
	//	for (Tile tile : objects) {
	//		if (within(point, tile)) return tile;
	//	}
	//	distance += 0.1f;
	//	return search(ray, distance, objects);
	//}
};