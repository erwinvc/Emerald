#pragma once
#include "entities/entity.h"
#include "util/uuid.h"
#include "assets/mesh.h"
#include "component.h"

namespace emerald {
	class TransformComponent : public Component {
		RTTI_DERIVED_CLASS_DECL(TransformComponent, Component);
	public:
		glm::vec3 m_position = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 m_rotation = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 m_scale = glm::vec3(1.0f, 1.0f, 1.0f);
	};

	class SceneGraphComponent : public Component {
		RTTI_DERIVED_CLASS_DECL(SceneGraphComponent, Component);
	public:
		SceneGraphComponent() : Component(), m_parent(nullptr) {}
		SceneGraphComponent(SceneGraphComponent* parent)
			: m_parent(parent) {
		}
		~SceneGraphComponent();

		void setParent(SceneGraphComponent* parent);
		void addChild(SceneGraphComponent* child);
		void setParent(Entity parent);
		void addChild(Entity child);
		void sortChildrenBasedOnIndex();

		const std::vector<SceneGraphComponent*>& getChildren() const { return m_children; }

	private:
		friend class HierarchyTree;

		bool m_isOpenInHierarchy = false; //This most definitely should not be stored here but it's the only way we can keep the HierarchyTree immediate mode...
		SceneGraphComponent* m_parent = nullptr;
		std::vector<SceneGraphComponent*> m_children;
		uint32_t m_id = 0; //For ImGui
		uint32_t m_treeIndex = 0; //Index in the entire tree
		uint32_t m_index = 0; //Index in the parent's children vector
		void removeChild(SceneGraphComponent* child);
	};

	class NameComponent : public Component {
		RTTI_DERIVED_CLASS_DECL(NameComponent, Component);
	public:
		std::string m_name;

		NameComponent(const std::string& name)
			: Component(), m_name(name) {
		}
	};

	class MeshRendererComponent : public Component {
		RTTI_DERIVED_CLASS_DECL(MeshRendererComponent, Component);
	public:
		Ref<Mesh> m_mesh;

		MeshRendererComponent() = default;
		MeshRendererComponent(const Ref<Mesh>& mesh)
			: m_mesh(mesh) {
		}
	};
}