#include "stdafx.h"

ClientWorld::ClientWorld() {
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

	m_blockEntityModel = NEW(Model(mesh));
}

ClientWorld::~ClientWorld() {
	DELETE(m_blockEntityModel);
}

void ClientWorld::Update(const TimeStep& time) {
	m_time++;
	if (m_rightClickDelayTimer > 0)	m_rightClickDelayTimer -= 0.016f;
	//for (int i = 0; i < m_entities.size(); i++) {
	//	if (i == GetClient()->GetPlayerID()) continue;
	//	auto& entity = m_entities[i];
	//	if (entity.active) {
	//		//entity.position += entity.velocity;
	//	}
	//}

	m_hasHoveredBlock = false;
	BlockIterator iter(Camera::active->transform.position, Utils::RotationToDirection(Camera::active->transform.rotation));
	glm::vec3 previousBlock;
	float scroll = GetMouse()->GetScroll().y;
	static int32 blockCount = 6;
	if (scroll > 0) {
		if (++m_selectedBlock > blockCount) m_selectedBlock = 1;
	} else if (scroll < 0) {
		if (--m_selectedBlock < 1) m_selectedBlock = blockCount;
	}

	for (; iter.t() < 5; ++iter) {
		if (GetBlock(*iter) != 0) {
			if (GetMouse()->ButtonJustDown(VK_MOUSE_LEFT)) {
				GetClient()->SetBlock(*iter, 0);
			} else if (GetMouse()->ButtonDown(VK_MOUSE_RIGHT) && m_rightClickDelayTimer <= 0) {
				glm::vec3& pos = Camera::active->transform.position;
				AABB player(pos.x - 0.3f, pos.y - 1.42f, pos.z - 0.3f, pos.x + 0.3f, pos.y + 0.38f, pos.z + 0.3f);
				BlockPos blockPos = ToBlockPosition(previousBlock);
				AABB block(blockPos);
				if (!player.Intersects(block)) {
					GetClient()->SetBlock(previousBlock, m_selectedBlock);
					m_rightClickDelayTimer = 0.2f;
				}
			}
			m_hoveredBlock = *iter;
			m_hasHoveredBlock = true;
			break;
		}
		previousBlock = *iter;
	}
}

void ClientWorld::RenderChunks(Shader* shader) {
	glDisable(GL_CULL_FACE);

	shader->Bind();
	for (auto it = m_chunks.begin(); it != m_chunks.end(); it++) {
		shader->Set("_ChunkPos", glm::vec3(it->second.m_position) * glm::vec3(CHUNK_SIZE));
		it->second.Draw();
	}
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
}

void ClientWorld::RenderGeometry(HDRPipeline* pipeline) {
	static Timer timer;
	m_chunkMaterial->Bind();
	m_chunkMaterial->GetShader()->Set("_Time", timer.Get());
	for (auto it = m_chunks.begin(); it != m_chunks.end(); it++) {
		m_chunkPosition = glm::vec3(it->second.m_position) * glm::vec3(CHUNK_SIZE);
		m_chunkMaterial->BindInstance();
		it->second.Draw();
	}

	m_blockEntityMaterial->Bind();
	for (int i = 0; i < m_entities.size(); i++) {
		if (i == GetClient()->GetPlayerID()) continue;
		Transform transform;
		transform.size = glm::vec3(0.25f, 0.25f, 0.25f);
		if (m_entities[i].active) {
			transform.position = m_entities[i].position;
			transform.rotation = glm::vec3(m_entities[i].rotation.x, m_entities[i].rotation.y, -m_entities[i].rotation.z);
			m_blockEntityModel->Draw(transform.GetTransformationMatrix());
		}
	}

	if (m_hasHoveredBlock) {
		pipeline->Bounds(m_hoveredBlock + glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.501f, 0.501f, 0.501f), Color(0.0f, 0.0f, 0.0f, 0.85f), false);
		pipeline->Bounds(m_hoveredBlock + glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.503f, 0.503f, 0.503f), Color(0.0f, 0.0f, 0.0f, 0.85f), false);
		pipeline->Bounds(m_hoveredBlock + glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.505f, 0.505f, 0.505f), Color(0.0f, 0.0f, 0.0f, 0.85f), false);
	}
}

uint8 ClientWorld::GetBlock(const glm::vec3& blockPosition) const {
	auto chunkPosition = ToChunkPosition(blockPosition);
	auto itr = m_chunks.find(chunkPosition);
	if (itr == m_chunks.cend()) {
		return 0;
	}
	return itr->second.GetBlockFast(ToLocalBlockPosition(blockPosition));
}

void ClientWorld::SetChunkDirty(const ChunkPos& chunkPosition) {
	auto itr = m_chunks.find(chunkPosition);
	if (itr != m_chunks.cend()) {
		itr->second.m_dirty = true;
	}
}

void ClientWorld::SetBlock(const glm::vec3& blockPosition, uint8 voxel) {
	auto chunkPosition = ToChunkPosition(blockPosition);
	auto itr = m_chunks.find(chunkPosition);
	auto local = ToLocalBlockPosition(blockPosition);
	if (itr != m_chunks.cend()) {
		itr->second.SetBlock(local, voxel);
		itr->second.m_dirty = true;
	}
}

void ClientWorld::UpdateEntity(uint32 id, const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& velocity) {
	EntityState& entity = m_entities[id];
	if (!entity.active) {
		return;
	}
	entity.lastPosition = entity.position;
	entity.lastRotation = entity.rotation;
	entity.position = position;
	entity.rotation = rotation;
	//entity.velocity = velocity;
}

void ClientWorld::AddEntity(uint32 id, const glm::vec3& position, String_t name, const glm::vec3& rotation, const glm::vec3& velocity) {
	m_entities[id].active = true;
	m_entities[id].position = position;
	strcpy(m_entities[id].name, name);
	m_entities[id].rotation = rotation;
}
