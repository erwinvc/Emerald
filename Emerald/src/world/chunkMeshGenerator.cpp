#include "stdafx.h"

bool ChunkMeshGenerator::MakeFace(BlockState* block, Chunk& chunk, glm::vec3&& position) {
	BlockState* blockState = nullptr;
	chunk.GetBlock(position, blockState);

	if (!blockState) return true;
	uint8 compareBlock = blockState->blockID;

	if (compareBlock == 0) {
		return true;
		//} else if (compareVoxel != thisId) {
		//	return true;
	}

	return false;
}

ChunkMesh ChunkMeshGenerator::MakeChunkMesh(Chunk& chunk) {
	static bool FRONT_FACE[] = { 1, 1, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1 };
	static bool LEFT_FACE[] = { 0, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1 };
	static bool BACK_FACE[] = { 0, 1, 0, 1, 1, 0, 1, 0, 0, 0, 0, 0 };
	static bool RIGHT_FACE[] = { 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 0 };
	static bool TOP_FACE[] = { 1, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1, 1 };
	static bool BOTTOM_FACE[] = { 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1 };

	Timer timer;
	ChunkMesh mesh;
	for (int y = 0; y < CHUNK_SIZE; y++) {
		for (int z = 0; z < CHUNK_SIZE; z++) {
			for (int x = 0; x < CHUNK_SIZE; x++) {
				glm::ivec3 voxelPosition(x, y, z);
				BlockState* blockState = nullptr;
				uint8 block = 0;
				if (chunk.GetBlock(voxelPosition, blockState)) block = blockState->blockID;
				if (block > 0) {
					// Bottom chunk face NEGATIVE Y
					if (MakeFace(blockState, chunk, { x, y - 1, z })) {
						mesh.AddFace(BOTTOM_FACE, BlockSide::DOWN, voxelPosition, block);
					}

					// Top chunk face POSITIVE Y
					if (MakeFace(blockState, chunk, { x, y + 1, z })) {
						mesh.AddFace(TOP_FACE, BlockSide::UP, voxelPosition, block);
					}

					// Back chunk face NEGATIVE Z
					if (MakeFace(blockState, chunk, { x, y, z - 1 })) {
						mesh.AddFace(BACK_FACE, BlockSide::NORTH, voxelPosition, block);
					}

					// Front chunk face POSITIVE Z
					if (MakeFace(blockState, chunk, { x, y, z + 1 })) {
						mesh.AddFace(FRONT_FACE, BlockSide::SOUTH, voxelPosition, block);
					}
					
					// Left voxel face NEGATIVE X
					if (MakeFace(blockState, chunk, { x - 1, y, z })) {
						mesh.AddFace(LEFT_FACE, BlockSide::WEST, voxelPosition, block);
					}

					// Right chunk face POSITIVE X
					if (MakeFace(blockState, chunk, { x + 1, y, z })) {
						mesh.AddFace(RIGHT_FACE, BlockSide::EAST, voxelPosition, block);
					}
				}
			}
		}
	}
	timer.Print();
	return mesh;
}