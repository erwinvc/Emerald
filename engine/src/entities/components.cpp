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
	RTTI_CLASS_DEF(MeshRendererComponent);

	void TransformComponent::updateLocalTransform() {
		m_localTransform = glm::mat4(1.0f);

		m_localTransform = glm::translate(m_localTransform, m_position);

		m_localTransform = glm::rotate(m_localTransform, m_rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		m_localTransform = glm::rotate(m_localTransform, m_rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		m_localTransform = glm::rotate(m_localTransform, m_rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));

		m_localTransform = glm::scale(m_localTransform, m_scale);

		m_needsUpdate = false;
	}

	void TransformComponent::updateGlobalTransform(const glm::mat4& parentTransform) {
		if (m_needsUpdate) updateLocalTransform();
		m_globalTransform = parentTransform * m_localTransform;
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