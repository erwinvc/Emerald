#pragma once
#include "component.h"
#include "rtti.h"
#include "ecs/entity.h"

namespace emerald {
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
}