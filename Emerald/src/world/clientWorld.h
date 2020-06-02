#pragma once

struct ReusableBlockEntity {
	bool m_inUse = false;
	Entity m_entity;
};

class ClientWorld {
private:
	glm::vec3 m_hoveredBlock;
	bool m_hasHoveredBlock;
	Material* m_chunkMaterial;
	glm::vec3 m_chunkPosition;
	ReusableBlockEntity m_blockEntities[16];
	Model* m_blockEntityModel;
	Material* m_blockEntityMaterial;
	vector<EntityState> m_entities;
	Entity* m_skybox;
	uint64 m_time = 0;
	int m_selectedBlock = 1;
	float m_rightClickDelayTimer = 0;
public:
	unordered_map<glm::ivec3, Chunk> m_chunks;
	ClientWorld() {
		m_entities.resize(32);

		Shader* shader = GetShaderManager()->Get("Chunk");
		shader->Bind();
		shader->Set("_Albedo", 0);
		shader->Set("_Normal", 1);
		shader->Set("_Metallic", 2);
		shader->Set("_Roughness", 3);
		shader->Set("_Emission", 4);

		m_chunkMaterial = GetMaterialManager()->Create("Chunk", shader);
		m_chunkMaterial->AddOnInstanceCallback(NEW(MaterialCallbackPtr("_ChunkPos", &m_chunkPosition)));
		m_chunkMaterial->AddOnBindCallback(NEW(MaterialCallbackTextureArray("_Albedo", GetTileMaterialManager()->GetAlbedo(), 0)));
		m_chunkMaterial->AddOnBindCallback(NEW(MaterialCallbackTextureArray("_Normal", GetTileMaterialManager()->GetNormal(), 1)));
		m_chunkMaterial->AddOnBindCallback(NEW(MaterialCallbackTextureArray("_Metallic", GetTileMaterialManager()->GetMetallic(), 2)));
		m_chunkMaterial->AddOnBindCallback(NEW(MaterialCallbackTextureArray("_Roughness", GetTileMaterialManager()->GetRoughness(), 3)));
		m_chunkMaterial->AddOnBindCallback(NEW(MaterialCallbackTextureArray("_Emission", GetTileMaterialManager()->GetEmission(), 4)));

		Mesh* mesh = GetAssetManager()->Get<Model>("Cube")->GetMeshes()[0]->Copy();
		m_blockEntityMaterial = GetMaterialManager()->Create("BlockEntity", GetShaderManager()->Get("Geometry"));
		m_blockEntityMaterial->AddOnInstanceCallback(NEW(MaterialCallbackPtr("_ChunkPos", &m_chunkPosition)));
		m_blockEntityMaterial->AddOnBindCallback(NEW(MaterialCallbackTexture("_Albedo", GetAssetManager()->Get<Texture>("Dirt"), 0)));
		m_blockEntityMaterial->AddOnBindCallback(NEW(MaterialCallbackTexture("_Normal", GetAssetManager()->Get<Texture>("DirtNormal"), 1)));
		m_blockEntityMaterial->AddOnBindCallback(NEW(MaterialCallbackTexture("_Roughness", GetAssetManager()->Get<Texture>("DirtRoughness"), 2)));
		m_blockEntityMaterial->AddOnBindCallback(NEW(MaterialCallbackTexture("_Metallic", GetAssetManager()->Get<Texture>("DirtMetallic"), 3)));
		mesh->SetMaterial(m_blockEntityMaterial);

		m_blockEntityModel = new Model(mesh);
		for (int i = 0; i < NUMOF(m_blockEntities); i++) {
			m_blockEntities[i].m_entity.m_model = m_blockEntityModel;
		}

		m_skybox = NEW(Entity(NEW(Model(mesh->Copy()))));
		Material* skyboxMat = GetMaterialManager()->Create("Skybox", GetShaderManager()->Get("Geometry"));
		skyboxMat->AddOnBindCallback(NEW(MaterialCallbackTexture("_Albedo", GetTextureManager()->GetBlackTexture(), 0)));
		m_skybox->m_model->SetMaterial(skyboxMat);
		m_skybox->transform.size = glm::vec3(32, 32, 32);
	}
	~ClientWorld() {

	}

	void AddBlockEntity(glm::ivec3 position) {
		//for (int i = 0; i < NUMOF(m_blockEntities); i++) {
		//	if (!m_blockEntities[i].m_inUse) {
		//		Entity& entity = m_blockEntities[i].m_entity;
		//		entity.transform.position = glm::vec3(0.5f, 0.5f, 0.5f) + glm::vec3(position);
		//
		//		entity.transform.size = glm::vec3(0.5f, 0.5f, 0.5f);
		//		Tween::To(entity.transform.size, glm::vec3(0, 0, 0), 2.0f)->SetOnComplete([this, i]
		//		{
		//			LOG("Done %f", m_blockEntities[i].m_entity.transform.size.x);
		//			m_blockEntities[i].m_inUse = false;
		//		});
		//
		//		m_blockEntities[i].m_inUse = true;
		//		return;
		//	}
		//}
		//LOG("No free spot found");
	}

	bool DoesChunkExist(const glm::ivec3& pos) {
		return m_chunks.find(pos) != m_chunks.end();
	}

	void Update(const TimeStep& time);

	void RenderChunks(Shader* shader) {
		glDisable(GL_CULL_FACE);

		shader->Bind();
		for (auto it = m_chunks.begin(); it != m_chunks.end(); it++) {
			shader->Set("_ChunkPos", glm::vec3(it->second.m_position) * glm::vec3(CHUNK_SIZE));
			it->second.Draw();
		}
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
	}

	void RenderGeometry(HDRPipeline* pipeline);

	uint8 GetBlock(const glm::vec3& blockPosition) const {
		auto chunkPosition = ToChunkPosition(blockPosition);
		auto itr = m_chunks.find(chunkPosition);
		if (itr == m_chunks.cend()) {
			return 0;
		}
		return itr->second.GetBlockFast(ToLocalBlockPosition(blockPosition));
	}

	void SetChunkDirty(const ChunkPos& chunkPosition) {
		auto itr = m_chunks.find(chunkPosition);
		if (itr != m_chunks.cend()) {
			itr->second.m_dirty = true;
		}
	}

	void SetBlock(const glm::vec3& blockPosition, uint8 voxel) {
		glm::vec3& pos = Camera::active->transform.position;
		AABB player(pos.x - 0.3f, pos.y - 1.62f, pos.z - 0.3f, pos.x + 0.3f, pos.y + 0.18f, pos.z + 0.3f);
		BlockPos blockPos = ToBlockPosition(blockPosition);
		AABB block(blockPos);
		if (player.Intersects(block)) return;

		auto chunkPosition = ToChunkPosition(blockPosition);
		auto itr = m_chunks.find(chunkPosition);
		auto local = ToLocalBlockPosition(blockPosition);
		if (itr != m_chunks.cend()) {
			itr->second.SetBlock(local, voxel);
			itr->second.m_dirty = true;
		}
	}

	void SetTime(uint64 time) {
		m_time = time;
	}

	uint64 GetTime() { return m_time; }

	void Clear() {
		m_chunks.clear();
	}

	void AddEntity(uint32 id, const glm::vec3& position, const glm::vec3& rotation);

	void UpdateEntity(uint32 id, const glm::vec3& position, const glm::vec3& rotation);

	void RemoveEntity(uint32 id) {
		m_entities[id].active = false;
	}
};