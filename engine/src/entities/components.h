#pragma once
#include "entities/entity.h"
#include "util/uuid.h"
#include "assets/mesh.h"

namespace emerald {
	class Component {
	public:
		Entity m_entity;
	};

	class TransformComponent : public Component {
	public:
		glm::vec3 m_position = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 m_rotation = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 m_scale = glm::vec3(1.0f, 1.0f, 1.0f);
	};

	class SceneGraphComponent : public Component {
	public:
		SceneGraphComponent() : m_parent(nullptr) {}
		SceneGraphComponent(SceneGraphComponent* parent)
			: m_parent(parent) {
		}

		void setParent(SceneGraphComponent& parent);
		void setChild(SceneGraphComponent& child);
		void setParent(Entity parent);
		void setChild(Entity child);

		const std::vector<SceneGraphComponent*>& getChildren() const { return m_children; }

	private:
		friend class HierarchyTree;

		bool m_isOpenInHierarchy = false; //This most definitely should not be stored here but it's the only way we can keep the HierarchyTree immediate mode...
		SceneGraphComponent* m_parent = nullptr;
		std::vector<SceneGraphComponent*> m_children;

		void removeChild(SceneGraphComponent* child) {
			m_children.erase(std::remove(m_children.begin(), m_children.end(), child), m_children.end());
		}
	};

	class UUIDComponent : public Component {
	public:
		UUID m_uuid;

		UUIDComponent() = default;
		UUIDComponent(UUIDComponent* uuid)
			: m_uuid(uuid->m_uuid) {
		}
	};

	class NameComponent : public Component {
	public:
		std::string m_name;

		NameComponent(const std::string& name)
			: m_name(name) {
		}
	};

	class MeshRendererComponent : public Component {
	public:
		Ref<Mesh> m_mesh;

		MeshRendererComponent() = default;
		MeshRendererComponent(Ref<Mesh> mesh)
			: m_mesh(mesh) {
		}
	};
}