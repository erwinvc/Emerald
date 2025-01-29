#pragma once
#include "component.h"
#include "engine/ecs/core/entity.h"
#include "engine/assets/model/mesh.h"
#include "componentRegistry.h"

namespace emerald {
	class MeshRendererComponent : public Component {
		COMPONENT_DECL(MeshRendererComponent)
	public:
		Ref<Mesh> m_mesh;

		MeshRendererComponent() = default;
		MeshRendererComponent(const Ref<Mesh>& mesh)
			: m_mesh(mesh) {
		}

		const ComponentTypeInfo& getComponentTypeInfo() override { return s_componentTypeInfo; }

		virtual nlohmann::json serialize() override;
		virtual void deserialize(const nlohmann::json& j) override;

	private:
		static inline ComponentTypeInfo s_componentTypeInfo = { "Mesh Renderer", ComponentCategory::GRAPHICS, true };
	};
}