#pragma once

class BlockState;
class ChunkMeshGenerator {
private:
	static bool MakeFace(BlockState* block, Chunk& chunk, glm::vec3&& position);

public:
	static ChunkMesh MakeChunkMesh(Chunk& chunk);
};