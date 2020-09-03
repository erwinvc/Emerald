#pragma once

class BlockState {
public:
	uint8 blockID = 0;
	//uint8 dDown = 0;	// NEGATIVE Y
	//uint8 dUp = 0;		// POSITIVE Y
	//uint8 dNorth = 0;	// NEGATIVE Z
	//uint8 dSouth = 0;	// POSITIVE Z
	//uint8 dWest = 0;	// NEGATIVE X
	//uint8 dEast = 0;	// POSITIVE X

	void operator=(const uint32& v) = delete;

	bool IsSolid() { return blockID != 0; }

	AABB GetAABB() {
		//return { ((float)dWest / 16.0f), ((float)dDown / 16.0f), ((float)dNorth / 16.0f), 1.0f - ((float)dEast / 16.0f), 1.0f - ((float)dUp / 16.0f), 1.0f - ((float)dSouth / 16.0f) };
		return { 0, 0, 0, 1, 1, 1 };
	}
	BlockState() {}
	BlockState(uint8 id) : blockID(id) {}
};