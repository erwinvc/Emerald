#pragma once

struct ReusableBlockEntity {
	bool m_inUse = false;
	Entity m_entity;
};

class World {
private:
	glm::vec3 m_hoveredBlock;
	bool m_hasHoveredBlock;
	Material* m_chunkMaterial;
	glm::vec3 m_chunkPosition;
	ReusableBlockEntity m_blockEntities[16];
	Model* m_blockEntityModel;
public:
	unordered_map<glm::ivec3, Chunk> m_chunks;
	World() {
		//for (int z = -1; z < 1; z++) {
		//	for (int x = -1; x < 1; x++) {
		//		for (int y = -16; y < 0; y++) {
		//			m_chunks.emplace(glm::ivec3(x, y, z), Chunk(glm::ivec3(x, y, z), this));
		//		}
		//	}
		//}
		//
		//for (int z = -1; z < 1; z++) {
		//	for (int x = -1; x < 1; x++) {
		//		m_chunks.emplace(glm::ivec3(x, 0, z), Chunk(glm::ivec3(x, 0, z), this, false));
		//	}
		//}
		//m_chunks.emplace(glm::ivec3(0, 0, 0), Chunk(glm::ivec3(0, 0, 0), this));

		Shader* shader = GetShaderManager()->Get("Chunk");
		shader->Bind();
		shader->Set("_Albedo", 0);
		shader->Set("_Normal", 1);
		shader->Set("_Metallic", 2);
		shader->Set("_Roughness", 3);

		m_chunkMaterial = GetMaterialManager()->Create("Chunk", shader);
		m_chunkMaterial->AddOnInstanceCallback(NEW(MaterialCallbackPtr("_ChunkPos", &m_chunkPosition)));
		m_chunkMaterial->AddOnBindCallback(NEW(MaterialCallbackTexture("_Albedo", GetAssetManager()->Get<Texture>("Dirt"), 0)));
		m_chunkMaterial->AddOnBindCallback(NEW(MaterialCallbackTexture("_Normal", GetAssetManager()->Get<Texture>("DirtNormal"), 1)));
		m_chunkMaterial->AddOnBindCallback(NEW(MaterialCallbackTexture("_Metallic", GetAssetManager()->Get<Texture>("DirtMetallic"), 2)));
		m_chunkMaterial->AddOnBindCallback(NEW(MaterialCallbackTexture("_Roughness", GetAssetManager()->Get<Texture>("DirtRoughness"), 3)));

		Mesh* mesh = GetAssetManager()->Get<Model>("Cube")->GetMeshes()[0]->Copy();
		Material* material = GetMaterialManager()->Create("BlockEntity", GetShaderManager()->Get("Geometry"));
		material->AddOnInstanceCallback(NEW(MaterialCallbackPtr("_ChunkPos", &m_chunkPosition)));
		material->AddOnBindCallback(NEW(MaterialCallbackTexture("_Albedo", GetAssetManager()->Get<Texture>("Dirt"), 0)));
		material->AddOnBindCallback(NEW(MaterialCallbackTexture("_Normal", GetAssetManager()->Get<Texture>("DirtNormal"), 1)));
		material->AddOnBindCallback(NEW(MaterialCallbackTexture("_Roughness", GetAssetManager()->Get<Texture>("DirtRoughness"), 2)));
		material->AddOnBindCallback(NEW(MaterialCallbackTexture("_Metallic", GetAssetManager()->Get<Texture>("DirtMetallic"), 3)));
		mesh->SetMaterial(material);

		m_blockEntityModel = new Model(mesh);
		for (int i = 0; i < NUMOF(m_blockEntities); i++) {
			m_blockEntities[i].m_entity.m_model = m_blockEntityModel;
		}
	}
	~World() {

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

	void Update(const TimeStep& time) {
		m_hasHoveredBlock = false;
		BlockIterator iter(Camera::active->transform.position, Utils::RotationToDirection(Camera::active->transform.rotation));
		glm::vec3 previousBlock;
		for (; iter.t() < 5; ++iter) {
			if (GetBlock(*iter) == 1) {
				if (GetMouse()->ButtonJustDown(VK_MOUSE_LEFT)) {
					SetBlock(*iter, 0);
				}
				if (GetMouse()->ButtonJustDown(VK_MOUSE_RIGHT)) {
					SetBlock(previousBlock, 1);
				}
				m_hoveredBlock = *iter;
				m_hasHoveredBlock = true;
				break;
			}
			previousBlock = *iter;
		}
		auto chunkPosition = ToChunkPosition(Camera::active->transform.position);
		auto localPosition = ToLocalBlockPosition(Camera::active->transform.position);
		bool a = GetBlock(Camera::active->transform.position - glm::vec3(0.0f, 0.2f, 0.0f)) == 1;
		//LOG("%s %d %d %d | %d %d %d", a ? "solid" : "not solid", chunkPosition.x, chunkPosition.y, chunkPosition.z, localPosition.x, localPosition.y, localPosition.z);
	}
	void RenderChunks(Shader* shader) {
		glDisable(GL_CULL_FACE);

		for (auto it = m_chunks.begin(); it != m_chunks.end(); it++) {
			shader->Set("_ChunkPos", glm::vec3(it->second.m_position) * glm::vec3(CHUNK_SIZE));
			it->second.Draw();
		}
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
	}
	void RenderGeometry(HDRPipeline* pipeline) {
		m_chunkMaterial->Bind();
		for (auto it = m_chunks.begin(); it != m_chunks.end(); it++) {
			m_chunkPosition = glm::vec3(it->second.m_position) * glm::vec3(CHUNK_SIZE);
			m_chunkMaterial->BindInstance();
			it->second.Draw();
		}

		for (auto& b : m_blockEntities) if (b.m_inUse) b.m_entity.Draw();

		if (m_hasHoveredBlock) {
			pipeline->Bounds(m_hoveredBlock + glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.501f, 0.501f, 0.501f), Color(0.0f, 0.0f, 0.0f, 0.85f), false);
			pipeline->Bounds(m_hoveredBlock + glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.503f, 0.503f, 0.503f), Color(0.0f, 0.0f, 0.0f, 0.85f), false);
			pipeline->Bounds(m_hoveredBlock + glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.505f, 0.505f, 0.505f), Color(0.0f, 0.0f, 0.0f, 0.85f), false);
		}
	}

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
			AddBlockEntity(blockPosition);
			itr->second.SetBlock(local, voxel);
			itr->second.m_dirty = true;
		}
	}

	//friend void to_json(nlohmann::json& j, const World& w) {
	//	j = nlohmann::json({ "grid", *w.m_grid });
	//}
};