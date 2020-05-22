#include "stdafx.h"

ChunkMesh ChunkMeshGenerator::MakeChunkMesh(const Chunk& chunk) {
	static float FRONT_FACE[] = { 1, 1, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1 };
	static float LEFT_FACE[] = { 0, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1 };
	static float BACK_FACE[] = { 0, 1, 0, 1, 1, 0, 1, 0, 0, 0, 0, 0 };
	static float RIGHT_FACE[] = { 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 0 };
	static float TOP_FACE[] = { 1, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1, 1 };
	static float BOTTOM_FACE[] = { 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1 };

	ChunkMesh mesh;
	// sf::Clock clock;
	// static int count = 0;
	// static float totalTime = 0;
	for (int y = 0; y < CHUNK_SIZE; y++) {
		for (int z = 0; z < CHUNK_SIZE; z++) {
			for (int x = 0; x < CHUNK_SIZE; x++) {
				// If it is "not air"
				glm::ivec3 voxelPosition(x, y, z);
				uint8 voxel = chunk.GetBlock(voxelPosition);
				if (voxel > 0) {
					// Left voxel face
					if (MakeFace(voxel, chunk.GetBlock({ x - 1, y, z }))) {
						mesh.AddFace(LEFT_FACE, BlockSide::LEFT, voxelPosition);
					}

					// Right chunk face
					if (MakeFace(voxel, chunk.GetBlock({ x + 1, y, z }))) {
						mesh.AddFace(RIGHT_FACE, BlockSide::RIGHT, voxelPosition);
					}

					// Front chunk face
					if (MakeFace(voxel, chunk.GetBlock({ x, y, z + 1 }))) {
						mesh.AddFace(FRONT_FACE, BlockSide::FRONT, voxelPosition);
					}

					// Back chunk face
					if (MakeFace(voxel, chunk.GetBlock({ x, y, z - 1 }))) {
						mesh.AddFace(BACK_FACE, BlockSide::BACK, voxelPosition);
					}

					// Bottom chunk face
					if (MakeFace(voxel, chunk.GetBlock({ x, y - 1, z }))) {
						mesh.AddFace(BOTTOM_FACE, BlockSide::BOTTOM, voxelPosition);
					}

					// Top chunk face
					if (MakeFace(voxel, chunk.GetBlock({ x, y + 1, z }))) {
						mesh.AddFace(TOP_FACE, BlockSide::TOP, voxelPosition);
					}
				}
			}
		}
	}
	return mesh;
}