#pragma once

class AABB {
public:
	float minX = 0;
	float minY = 0;
	float minZ = 0;
	float maxX = 0;
	float maxY = 0;
	float maxZ = 0;

	AABB(float mnX, float mnY, float mnZ, float mxX, float mxY, float mxZ) : minX(mnX), minY(mnY), minZ(mnZ), maxX(mxX), maxY(mxY), maxZ(mxZ) {}

	bool Intersects(const AABB& other) {
		return Intersects(other.minX, other.minY, other.minZ, other.maxX, other.maxY, other.maxZ);
	}

	bool Intersects(float x1, float y1, float z1, float x2, float y2, float z2) {
		return minX < x2 && maxX > x1 && minY < y2 && maxY > y1 && minZ < z2 && maxZ > z1;
	}

	bool Intersects(glm::vec3 min, glm::vec3 max) {
		return Intersects(Math::Min(min.x, max.x), Math::Min(min.y, max.y), Math::Min(min.z, max.z), Math::Max(min.x, max.x), Math::Max(min.y, max.y), Math::Max(min.z, max.z));
	}
};