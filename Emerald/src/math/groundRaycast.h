#pragma once

class Camera;

class GroundRaycast {
public:
	static glm::vec3 GetMousePosition(Camera* camera = nullptr);
	static glm::vec3 GetScreenPosition(glm::vec2 position, Camera* camera = nullptr);
	static glm::vec3 GetGroundPosition(const glm::vec3& ray, float height, Camera* camera = nullptr);
	static glm::ivec2 GetTile();

private:
	static glm::vec3 CalculateMouseRay(Camera* camera);
	static glm::vec3 CalculateScreenRay(const glm::vec2& position, Camera* camera);
	static glm::vec4 ToEyeCoords(const glm::vec4& clipCoords, Camera* camera);
	static glm::vec3 ToWorldCoords(const glm::vec4& eyeCoords, Camera* camera);

	//Tile search(glm::vec3 ray, float distance, Tile[][] objects) {
	//	if (distance > RAY_RANGE) return null;
	//	glm::vec3F point = getPointOnRay(ray, distance);
	//	for (Tile[] t : objects) {
	//		for (Tile tile : t) {
	//			if (within(point, tile)) return tile;
	//		}
	//	}
	//	distance += 0.1f;
	//	return search(ray, distance, objects);
	//}
	//
	//Tile search(glm::vec3F ray, float distance, ArrayList<Tile> objects) {
	//	if (distance > RAY_RANGE) return null;
	//	glm::vec3F point = getPointOnRay(ray, distance);
	//	for (Tile tile : objects) {
	//		if (within(point, tile)) return tile;
	//	}
	//	distance += 0.1f;
	//	return search(ray, distance, objects);
	//}
};