#pragma once

enum class BlockSide;
class BlockPos {
public:
	int32 x;
	int32 y;
	int32 z;

	BlockPos() : x(0), y(0), z(0) {}
	BlockPos(int32 _x, int32 _y, int32 _z) : x(_x), y(_y), z(_z) {}
	BlockPos(const glm::ivec3& vec) : x(vec.x), y(vec.y), z(vec.z) {}

	BlockPos& operator=(const glm::ivec3& wp) {
		x = wp.x;
		y = wp.y;
		z = wp.z;
		return *this;
	}

	operator glm::ivec3() const { return { x, y, z }; }
	operator glm::vec3() const { return { x, y, z }; }

	BlockPos Offset(BlockSide blockSide, int amount = 1);
};