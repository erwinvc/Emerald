#include "eepch.h"
#include "sceneGraphComponent.h"
#include "engine/scene/sceneManager.h"
#include "utils/misc/utils.h"
#include "../core/ECSManager.h"

namespace emerald {
	RTTI_CLASS_DEF(SceneGraphComponent);
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
		setParent(ECSManager::ECS().getComponent<SceneGraphComponent>(parent));
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
		addChild(ECSManager::ECS().getComponent<SceneGraphComponent>(child));
	}

	void SceneGraphComponent::removeChild(SceneGraphComponent* child) {
		utils::eraseFromVector(m_children, child);
	}

	void SceneGraphComponent::sortChildrenBasedOnIndex() {
		std::sort(m_children.begin(), m_children.end(), [](SceneGraphComponent* a, SceneGraphComponent* b) {
			return a->m_index < b->m_index;
		});
	}

	void SceneGraphComponent::setEnabledRecursive(bool enabled) {
		ECSManager::ECS().setEntityEnabled(m_entity, enabled);
		for (auto child : m_children) {
			child->setEnabledRecursive(enabled);
		}
	}

	nlohmann::json SceneGraphComponent::serialize() {
		nlohmann::json json;
		json["parent"] = m_parent ? m_parent->m_entity : 0;
		json["children"] = nlohmann::json::array();
		for (auto child : m_children) {
			json["children"].push_back(child->m_entity);
		}
		return json;
	}

	SceneGraphComponent* SceneGraphComponent::deserialize(const nlohmann::json& json, Entity entity) {
		SceneGraphComponent* comp = ECSManager::ECS().addComponent<SceneGraphComponent>(entity);

		comp->setParent((Entity)json["parent"]);
		for (auto child : json["children"]) {
			comp->addChild((Entity)child);
		}

		return comp;
	}
}