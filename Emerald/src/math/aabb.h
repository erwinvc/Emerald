#pragma once

enum class BlockSide;
class AABB {
public:
	float minX = 0.0f;
	float minY = 0.0f;
	float minZ = 0.0f;
	float maxX = 0.0f;
	float maxY = 0.0f;
	float maxZ = 0.0f;

	AABB(float mnX, float mnY, float mnZ, float mxX, float mxY, float mxZ) : minX(mnX), minY(mnY), minZ(mnZ), maxX(mxX), maxY(mxY), maxZ(mxZ) {}
	AABB(const BlockPos& blockPos) : minX((float)blockPos.x), minY((float)blockPos.y), minZ((float)blockPos.z), maxX((float)blockPos.x + 1.0f), maxY((float)blockPos.y + 1.0f), maxZ((float)blockPos.z + 1.0f) {}

	bool Intersects(const AABB& other) {
		return Intersects(other.minX, other.minY, other.minZ, other.maxX, other.maxY, other.maxZ);
	}

	bool Intersects(float x1, float y1, float z1, float x2, float y2, float z2) {
		return minX < x2&& maxX > x1 && minY < y2&& maxY > y1 && minZ < z2&& maxZ > z1;
	}

	bool Intersects(glm::vec3 min, glm::vec3 max) {
		return Intersects(Math::Min(min.x, max.x), Math::Min(min.y, max.y), Math::Min(min.z, max.z), Math::Max(min.x, max.x), Math::Max(min.y, max.y), Math::Max(min.z, max.z));
	}

	AABB AddCoord(float x, float y, float z) {
		float nMinX = minX;
		float nMinY = minY;
		float nMinZ = minZ;
		float nMaxX = maxX;
		float nMaxY = maxY;
		float nMaxZ = maxZ;
		if (x < 0.0f) {
			nMinX += x;
		} else if (x > 0.0f) {
			nMaxX += x;
		}

		if (y < 0.0f) {
			nMinY += y;
		} else if (y > 0.0f) {
			nMaxY += y;
		}

		if (z < 0.0f) {
			nMinZ += z;
		} else if (z > 0.0f) {
			nMaxZ += z;
		}

		return { nMinX, nMinY, nMinZ, nMaxX, nMaxY, nMaxZ };
	}

	AABB Offset(float x, float y, float z) {
		return { minX + x, minY + y, minZ + z, maxX + x, maxY + y, maxZ + z };
	}

	AABB Offset(const glm::vec3& offset) {
		return { minX + offset.x, minY + offset.y, minZ + offset.z, maxX + offset.x, maxY + offset.y, maxZ + offset.z };
	}

	AABB Expand(const glm::vec3& expansion) {
		return { minX - expansion.x, minY - expansion.y, minZ - expansion.z, maxX + expansion.x, maxY + expansion.y, maxZ + expansion.z };
	}

	AABB Expand(float x, float y, float z) {
		return { minX - x, minY - y, minZ - z, maxX + x, maxY + y, maxZ + z };
	}

	AABB Copy() {
		return { minX, minY, minZ, maxX, maxY, maxZ };
	}

	bool CalculateXOffset(const AABB& other, float& offsetX) {
		if (other.maxY > minY && other.minY < maxY && other.maxZ > minZ && other.minZ < maxZ) {
			if (offsetX > 0.0f && other.maxX <= (minX + 0.02f)) {
				float d1 = minX - other.maxX;

				if (d1 < offsetX) {
					offsetX = d1 - 0.001f;
					if (d1 > 0.001f) return true;
				}
			} else if (offsetX < 0.0f && other.minX >= (maxX - 0.02f)) {
				float d0 = maxX - other.minX;

				if (d0 > offsetX) {
					offsetX = d0 + 0.001f;
					if (d0 < 0.001f) return true;
				}
			}
		}
		return false;
	}

	bool CalculateYOffset(const AABB& other, float& offsetY) {
		if (other.maxX > minX && other.minX < maxX && other.maxZ >minZ && other.minZ < maxZ) {
			if (offsetY > 0.0f && other.maxY <= (minY + 0.02f)) {
				float d1 = minY - other.maxY;

				if (d1 < offsetY) {
					offsetY = d1 - 0.001f;
					if (d1 > 0.001f) return true;
				}
			} else if (offsetY < 0.0f && other.minY >= (maxY - 0.02f)) {
				float d0 = maxY - other.minY;

				if (d0 > offsetY) {
					offsetY = d0 + 0.001f;
					if (d0 < 0.001f) return true;
				}
			}
		}
		return false;
	}


	bool CalculateZOffset(const AABB& other, float& offsetZ) {
		if (other.maxX > minX && other.minX < maxX && other.maxY > minY && other.minY < maxY) {
			if (offsetZ > 0.0f && other.maxZ <= (minZ + 0.02f)) {
				float d1 = minZ - other.maxZ;

				if (d1 < offsetZ) {
					offsetZ = d1 - 0.001f;
					if (d1 > 0.001f) return true;
				}
			} else if (offsetZ < 0.0f && other.minZ >= (maxZ - 0.02f)) {
				float d0 = maxZ - other.minZ;

				if (d0 > offsetZ) {
					offsetZ = d0 + 0.001f;
					if (d0 < 0.001f) return true;
				}
			}
		}
		return false;
	}

	glm::vec3 GetCenter() const {
		return glm::vec3(minX + (maxX - minX) * 0.5f, minY + (maxY - minY) * 0.5f, minZ + (maxZ - minZ) * 0.5f);
	}

	glm::vec3 GetSize() const {
		return glm::vec3(maxX - minX, maxY - minY, maxZ - minZ);
	}

	bool CalculateIntercept(BlockSide& blockside, const glm::vec3& vecA, const glm::vec3& vecB);

	bool IsClosest(bool success, const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3);
	bool CollideWithXPlane(glm::vec3& in, float x, glm::vec3 rayStart, glm::vec3 rayEnd) {
		in = rayStart;
		bool toRet = GetIntermediateWithXValue(in, rayEnd, x);
		return toRet && IntersectsWithYZ(in);
	}

	bool CollideWithYPlane(glm::vec3& in, float y, glm::vec3 rayStart, glm::vec3 rayEnd) {
		in = rayStart;
		bool toRet = GetIntermediateWithYValue(in, rayEnd, y);
		return toRet && IntersectsWithXZ(in);
	}

	bool CollideWithZPlane(glm::vec3& in, float z, glm::vec3 rayStart, glm::vec3 rayEnd) {
		in = rayStart;
		bool toRet = GetIntermediateWithZValue(in, rayEnd, z);
		return toRet && IntersectsWithXY(in);
	}

	bool IntersectsWithYZ(glm::vec3 vec) {
		return vec.y >= minY && vec.y <= maxY && vec.z >= minZ && vec.z <= maxZ;
	}

	bool IntersectsWithXZ(glm::vec3 vec) {
		return vec.x >= minX && vec.x <= maxX && vec.z >= minZ && vec.z <= maxZ;
	}

	bool IntersectsWithXY(glm::vec3 vec) {
		return vec.x >= minX && vec.x <= maxX && vec.y >= minY && vec.y <= maxY;
	}

	bool GetIntermediateWithXValue(glm::vec3& in, const glm::vec3& vec, float x);
	bool GetIntermediateWithYValue(glm::vec3& in, const glm::vec3& vec, float y);
	bool GetIntermediateWithZValue(glm::vec3& in, const glm::vec3& vec, float z);
};