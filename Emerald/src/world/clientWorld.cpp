#include "stdafx.h"

void ClientWorld::Update(const TimeStep& time) {
	m_time++;
	if (m_rightClickDelayTimer > 0)	m_rightClickDelayTimer -= 0.016f;
	for (int i = 0; i < m_entities.size(); i++) {
		auto& entity = m_entities[i];
		if (entity.active) {
			entity.position += entity.velocity / 60.0f;
		}
	}

	m_hasHoveredBlock = false;
	BlockIterator iter(Camera::active->transform.position, Utils::RotationToDirection(Camera::active->transform.rotation));
	glm::vec3 previousBlock;
	float scroll = GetMouse()->GetScroll().y;
	if (scroll > 0) {
		if (++m_selectedBlock > 3) m_selectedBlock = 1;
	} else if (scroll < 0) {
		if (--m_selectedBlock < 1) m_selectedBlock = 3;
	}

	for (; iter.t() < 5; ++iter) {
		if (GetBlock(*iter) != 0) {
			if (GetMouse()->ButtonJustDown(VK_MOUSE_LEFT)) {
				GetClient()->SetBlock(*iter, 0);
				//SetBlock(*iter, 0);
			} else if (GetMouse()->ButtonDown(VK_MOUSE_RIGHT) && m_rightClickDelayTimer <= 0) {
				GetClient()->SetBlock(previousBlock, m_selectedBlock);
				m_rightClickDelayTimer = 0.2f;
				//SetBlock(previousBlock, 1);
			}
			m_hoveredBlock = *iter;
			m_hasHoveredBlock = true;
			break;
		}
		previousBlock = *iter;
	}
}

void ClientWorld::RenderGeometry(HDRPipeline* pipeline) {
	m_chunkMaterial->Bind();
	for (auto it = m_chunks.begin(); it != m_chunks.end(); it++) {
		m_chunkPosition = glm::vec3(it->second.m_position) * glm::vec3(CHUNK_SIZE);
		m_chunkMaterial->BindInstance();
		it->second.Draw();
	}

	for (auto& b : m_blockEntities) if (b.m_inUse) b.m_entity.Draw();

	m_blockEntityMaterial->Bind();
	for (int i = 0; i < m_entities.size(); i++) {
		if (i == GetClient()->GetPlayerID()) continue;
		Transform transform;
		transform.size = glm::vec3(0.5f, 0.5f, 0.5f);
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

	//GL(glFrontFace(GL_CW));
	//m_skybox->transform.position.y = Camera::active->transform.position.y;
	//m_skybox->Draw();
	//GL(glFrontFace(GL_CCW));
}

void ClientWorld::UpdateEntity(uint32 id, const glm::vec3& position, const glm::vec3& rotation) {
	EntityState& entity = m_entities[id];
	if (!entity.active) {
		return;
	}
	entity.lastPosition = entity.position;
	entity.lastRotation = entity.rotation;
	entity.position = position;
	entity.rotation = rotation;
	entity.velocity = entity.position - entity.lastPosition;

	float currentTime = GetClient()->GetRealTimeSeconds();
	float time = currentTime - m_entities[id].lastUpdateTime;
	m_entities[id].lastUpdateTime = currentTime;

	entity.velocity *= time * 100;
}

void ClientWorld::AddEntity(uint32 id, const glm::vec3& position, const glm::vec3& rotation) {
	m_entities[id].active = true;
	m_entities[id].position = position;
	m_entities[id].rotation = rotation;
	m_entities[id].lastUpdateTime = GetClient()->GetRealTimeSeconds();
}
