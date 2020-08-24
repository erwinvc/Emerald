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

	if (blockState->dDown != 0) return true;
	if (blockState->dUp != 0) return true;
	if (blockState->dNorth != 0) return true;
	if (blockState->dSouth != 0) return true;
	if (blockState->dWest != 0) return true;
	if (blockState->dEast != 0) return true;
	if (block->dDown != 0) return true;
	if (block->dUp != 0) return true;
	if (block->dNorth != 0) return true;
	if (block->dSouth != 0) return true;
	if (block->dWest != 0) return true;
	if (block->dEast != 0) return true;
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
				uint8 dW = blockState->dWest;
				uint8 dE = blockState->dEast;
				uint8 dD = blockState->dDown;
				uint8 dU = blockState->dUp;
				uint8 dS = blockState->dSouth;
				uint8 dN = blockState->dNorth;
				if (block > 0) {
					// Bottom chunk face NEGATIVE Y
					if (MakeFace(blockState, chunk, { x, y - 1, z })) {
						mesh.AddFace(BOTTOM_FACE, BlockSide::DOWN, voxelPosition, block, dW, dE, dD, dU, dN, dS);
					}

					// Top chunk face POSITIVE Y
					if (MakeFace(blockState, chunk, { x, y + 1, z })) {
						mesh.AddFace(TOP_FACE, BlockSide::UP, voxelPosition, block, dW, dE, dD, dU, dN, dS);
					}

					// Back chunk face NEGATIVE Z
					if (MakeFace(blockState, chunk, { x, y, z - 1 })) {
						mesh.AddFace(BACK_FACE, BlockSide::NORTH, voxelPosition, block, dW, dE, dD, dU, dN, dS);
					}

					// Front chunk face POSITIVE Z
					if (MakeFace(blockState, chunk, { x, y, z + 1 })) {
						mesh.AddFace(FRONT_FACE, BlockSide::SOUTH, voxelPosition, block, dW, dE, dD, dU, dN, dS);
					}
					
					// Left voxel face NEGATIVE X
					if (MakeFace(blockState, chunk, { x - 1, y, z })) {
						mesh.AddFace(LEFT_FACE, BlockSide::WEST, voxelPosition, block, dW, dE, dD, dU, dN, dS);
					}

					// Right chunk face POSITIVE X
					if (MakeFace(blockState, chunk, { x + 1, y, z })) {
						mesh.AddFace(RIGHT_FACE, BlockSide::EAST, voxelPosition, block, dW, dE, dD, dU, dN, dS);
					}
				}
			}
		}
	}
	timer.Print();
	return mesh;
}