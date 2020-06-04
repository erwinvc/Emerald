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
};