#pragma once

class Camera;

class GroundRaycast {
public:
	static Vector3 GetMousePosition();
	static Vector3 GetScreenPosition(Vector2 position);
	static Vector3 GetGroundPosition(const Vector3& ray, float height);
	static Vector2I GetTile();

private:
	static Vector3 CalculateMouseRay();
	static Vector3 CalculateScreenRay(const Vector2& position);
	static Vector4 ToEyeCoords(const Vector4& clipCoords, const AssetRef<Camera>& camera);
	static Vector3 ToWorldCoords(const Vector4& eyeCoords, const AssetRef<Camera>& camera);

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