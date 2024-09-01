#include "eepch.h"
#include "components.h"
#include "scene/sceneManager.h"

namespace emerald {
	RTTI_CLASS_DEF(Component);
	RTTI_CLASS_DEF(SceneGraphComponent);
	RTTI_CLASS_DEF(TransformComponent);
	RTTI_CLASS_DEF(NameComponent);
	RTTI_CLASS_DEF(UUIDComponent);

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
		setParent(&SceneManager::getActiveScene()->getECS().getComponent<SceneGraphComponent>(parent));
	}

	void SceneGraphComponent::addChild(SceneGraphComponent* child) {
		if (child->m_parent) {
			child->m_parent->removeChild(child);
		}
		m_children.push_back(child);
		child->m_parent = this;
	}

	void SceneGraphComponent::addChild(Entity child) {
		addChild(&SceneManager::getActiveScene()->getECS().getComponent<SceneGraphComponent>(child));
	}
}