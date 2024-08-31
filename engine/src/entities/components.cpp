#include "eepch.h"
#include "components.h"
#include "scene/sceneManager.h"

namespace emerald {
	void SceneGraphComponent::setParent(SceneGraphComponent& parent) {
		if (m_parent) {
			m_parent->removeChild(this);
		}
		parent.m_children.push_back(this);
		m_parent = &parent;
	}

	void SceneGraphComponent::setParent(Entity parent) {
		setParent(SceneManager::getActiveScene()->getECS().getComponent<SceneGraphComponent>(parent));
	}

	void SceneGraphComponent::setChild(SceneGraphComponent& child) {
		if (child.m_parent) {
			child.m_parent->removeChild(&child);
		}
		m_children.push_back(&child);
		child.m_parent = this;
	}

	void SceneGraphComponent::setChild(Entity child) {
		setChild(SceneManager::getActiveScene()->getECS().getComponent<SceneGraphComponent>(child));
	}
}