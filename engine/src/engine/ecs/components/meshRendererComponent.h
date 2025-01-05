#pragma once
#include "component.h"
#include "engine/ecs/core/entity.h"
#include "engine/assets/model/mesh.h"

namespace emerald {
	class MeshRendererComponent : public Component {
		RTTI_DERIVED_CLASS_DECL(MeshRendererComponent, Component);
	public:
		Ref<Mesh> m_mesh;

		MeshRendererComponent() = default;
		MeshRendererComponent(const Ref<Mesh>& mesh)
			: m_mesh(mesh) {
		}

		const ComponentTypeInfo& getComponentTypeInfo() override { return s_componentTypeInfo; }

		virtual nlohmann::json serialize() override;
		static MeshRendererComponent* deserialize(const nlohmann::json& json, Entity entity);

	private:
		static inline ComponentTypeInfo s_componentTypeInfo = { "Mesh Renderer", ComponentCategory::GRAPHICS, true };
	};
}