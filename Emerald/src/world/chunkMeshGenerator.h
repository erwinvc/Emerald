#pragma once

class ChunkMeshGenerator {
private:
	static bool MakeFace(uint8 thisId, uint8 compareVoxel) {
		uint8 air = 0;

		if (compareVoxel == air) {
			return true;
		} else if (compareVoxel != thisId) {
			return true;
		}
		return false;
	}

public:
	static ChunkMesh MakeChunkMesh(const Chunk& chunk);
};