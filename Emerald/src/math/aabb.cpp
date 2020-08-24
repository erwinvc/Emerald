#include "stdafx.h"

bool AABB::CalculateIntercept(BlockSide& blockside, const glm::vec3& rayStart, const glm::vec3& rayEnd) {
	glm::vec3 vecMinX(0.0f, 0.0f, 0.0f);
	bool success = CollideWithXPlane(vecMinX, minX, rayStart, rayEnd);
	blockside = BlockSide::WEST;
	glm::vec3 vecMaxX(0.0f, 0.0f, 0.0f);
	bool collMaxX = CollideWithXPlane(vecMaxX, maxX, rayStart, rayEnd);

	if (collMaxX && IsClosest(success, rayStart, vecMinX, vecMaxX)) {
		vecMinX = vecMaxX;
		blockside = BlockSide::EAST;
		success = true;
	}

	collMaxX = CollideWithYPlane(vecMaxX, minY, rayStart, rayEnd);

	if (collMaxX && IsClosest(success, rayStart, vecMinX, vecMaxX)) {
		vecMinX = vecMaxX;
		blockside = BlockSide::DOWN;
		success = true;
	}

	collMaxX = CollideWithYPlane(vecMaxX, maxY, rayStart, rayEnd);

	if (collMaxX && IsClosest(success, rayStart, vecMinX, vecMaxX)) {
		vecMinX = vecMaxX;
		blockside = BlockSide::UP;
		success = true;
	}

	collMaxX = CollideWithZPlane(vecMaxX, minZ, rayStart, rayEnd);

	if (collMaxX && IsClosest(success, rayStart, vecMinX, vecMaxX)) {
		vecMinX = vecMaxX;
		blockside = BlockSide::NORTH;
		success = true;
	}

	collMaxX = CollideWithZPlane(vecMaxX, maxZ, rayStart, rayEnd);

	if (collMaxX && IsClosest(success, rayStart, vecMinX, vecMaxX)) {
		vecMinX = vecMaxX;
		blockside = BlockSide::SOUTH;
		success = true;
	}

	//return vec3d == null ? null : new RayTraceResult(vec3d, enumfacing);
	return success;
}

bool AABB::IsClosest(bool success, const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3) {
	return !success || glm::distance2(v1, v3) < glm::distance2(v1, v2);
}

bool AABB::GetIntermediateWithXValue(glm::vec3& in, const glm::vec3& vec, float x) {
	float d0 = vec.x - in.x;
	float d1 = vec.y - in.y;
	float d2 = vec.z - in.z;

	if (d0 * d0 < 0.0000001f) {
		return false;
	} else {
		float d3 = (x - in.x) / d0;
		bool toRet = d3 >= 0.0f && d3 <= 1.0f;
		if (toRet) in = glm::vec3(in.x + d0 * d3, in.y + d1 * d3, in.z + d2 * d3);
		return toRet;
	}
}

bool AABB::GetIntermediateWithYValue(glm::vec3& in, const glm::vec3& vec, float y) {
	float d0 = vec.x - in.x;
	float d1 = vec.y - in.y;
	float d2 = vec.z - in.z;

	if (d1 * d1 < 0.0000001f) {
		return false;
	} else {
		float d3 = (y - in.y) / d1;
		bool toRet = d3 >= 0.0f && d3 <= 1.0f;
		if (toRet) in = glm::vec3(in.x + d0 * d3, in.y + d1 * d3, in.z + d2 * d3);
		return toRet;
	}
}

bool AABB::GetIntermediateWithZValue(glm::vec3& in, const glm::vec3& vec, float z) {
	float d0 = vec.x - in.x;
	float d1 = vec.y - in.y;
	float d2 = vec.z - in.z;

	if (d2 * d2 < 0.0000001f) {
		return false;
	} else {
		float d3 = (z - in.z) / d2;
		bool toRet = d3 >= 0.0f && d3 <= 1.0f;
		if (toRet) in = glm::vec3(in.x + d0 * d3, in.y + d1 * d3, in.z + d2 * d3);
		return toRet;
	}
}