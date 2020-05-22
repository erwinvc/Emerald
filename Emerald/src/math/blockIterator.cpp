#include "stdafx.h"

BlockIterator::BlockIterator() : BlockIterator(glm::vec3(0, 0, 0), glm::vec3(1, 1, 1)) {
}

BlockIterator::BlockIterator(const glm::vec3& pos, const glm::vec3& direction) : size(glm::vec3(1.0f, 1.0f, 1.0f)),
	dir(glm::normalize(direction)),
	step(Math::Sign(dir.x), Math::Sign(dir.y), Math::Sign(dir.z)),
	tDelta(size.x / Math::Abs(dir.x),
	size.y / Math::Abs(dir.y),
	size.z / Math::Abs(dir.z)),
	voxel((int)(Math::Floor(pos.x / size.x)),
	(int)(Math::Floor(pos.y / size.y)),
	(int)(Math::Floor(pos.z / size.z))),
	currentT(0.0) {
	
	// These three sections compute how far we have to travel in the given
	// direction to reach the next voxel boundary along each dimension
	tNext.x = Math::Mod(pos.x, size.x);
	tNext.y = Math::Mod(pos.y, size.y);
	tNext.z = Math::Mod(pos.z, size.z);

	if (pos.x < 0) tNext.x += size.x;
	if (pos.y < 0) tNext.y += size.y;
	if (pos.z < 0) tNext.z += size.z;

	if (step.x == 1) tNext.x = size.x - tNext.x;
	if (step.y == 1) tNext.y = size.y - tNext.y;
	if (step.z == 1) tNext.z = size.z - tNext.z;

	// Convert these distances into values of t
	tNext.x /= Math::Abs(dir.x);
	tNext.y /= Math::Abs(dir.y);
	tNext.z /= Math::Abs(dir.z);

	if (!std::isfinite(tNext.x)) tNext.x = Math::MAX_FLOAT;
	if (!std::isfinite(tNext.y)) tNext.y = Math::MAX_FLOAT;
	if (!std::isfinite(tNext.z)) tNext.z = Math::MAX_FLOAT;
}

BlockIterator BlockIterator::operator++() {
	if (tNext.x < tNext.y) {
		if (tNext.x < tNext.z) {
			currentT = tNext.x;
			voxel.x += step.x;
			tNext.x += tDelta.x;
		} else {
			currentT = tNext.z;
			voxel.z += step.z;
			tNext.z += tDelta.z;
		}
	} else if (tNext.y < tNext.z) {
		currentT = tNext.y;
		voxel.y += step.y;
		tNext.y += tDelta.y;
	} else {
		currentT = tNext.z;
		voxel.z += step.z;
		tNext.z += tDelta.z;
	}

	return *this;
}

glm::ivec3 BlockIterator::operator*() {
	return voxel;
}

const glm::ivec3 BlockIterator::operator*() const {
	return voxel;
}