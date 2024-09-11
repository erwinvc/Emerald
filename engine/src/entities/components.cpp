#include "eepch.h"
#include "components.h"
#include "scene/sceneManager.h"
#include "../util/utils.h"
#include "../util/uuidGenerator.h"

namespace emerald {
	RTTI_CLASS_DEF(Component);
	RTTI_CLASS_DEF(SceneGraphComponent);
	RTTI_CLASS_DEF(TransformComponent);
	RTTI_CLASS_DEF(NameComponent);
	RTTI_CLASS_DEF(UUIDComponent);
	RTTI_CLASS_DEF(MeshRendererComponent);

	UUIDComponent::UUIDComponent() {
		m_uuid = UUIDGenerator::createFast();
	}

	SceneGraphComponent::~SceneGraphComponent() {
		if (m_parent) m_parent->removeChild(this);
		for (auto child : m_children) {
			child->m_parent = nullptr;
		}
		m_children.clear();
	}

	void SceneGraphComponent::setParent(SceneGraphComponent* parent) {
		if (m_parent) {
			m_parent->removeChild(this);
		}
		if (parent) {
			parent->m_children.push_back(this);
		}
		m_parent = parent;
	}

	void SceneGraphComponent::setParent(Entity parent) {
		setParent(SceneManager::getActiveScene()->getECS().getComponent<SceneGraphComponent>(parent));
	}

	void SceneGraphComponent::addChild(SceneGraphComponent* child) {
		if (child->m_parent) {
			child->m_parent->removeChild(child);
		}
		m_children.push_back(child);
		child->m_parent = this;

		uint32_t index = 0;
		for (auto& c : m_children)
			c->m_index = index++;
	}

	void SceneGraphComponent::addChild(Entity child) {
		addChild(SceneManager::getActiveScene()->getECS().getComponent<SceneGraphComponent>(child));
	}

	void SceneGraphComponent::removeChild(SceneGraphComponent* child) {
		utils::eraseFromVector(m_children, child);
	}

	void SceneGraphComponent::sortChildrenBasedOnIndex() {
		std::sort(m_children.begin(), m_children.end(), [](SceneGraphComponent* a, SceneGraphComponent* b) {
			return a->m_index < b->m_index;
		});

	}
}