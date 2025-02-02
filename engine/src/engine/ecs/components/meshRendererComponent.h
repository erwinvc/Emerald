#pragma once
#include "component.h"
#include "componentRegistry.h"
#include "core/common/assetRef.h"
#include "../../assets/model/model.h"

namespace emerald {
	class MeshRendererComponent : public Component {
		COMPONENT_DECL(MeshRendererComponent)
	public:
		AssetRef<Model> m_model;
		uint32_t m_submeshIndex = 0;
		MeshRendererComponent() = default;
		MeshRendererComponent(const AssetRef<Model>& model, uint32_t submeshIndex)
			: m_model(model), m_submeshIndex(submeshIndex) {
		}

		const ComponentTypeInfo& getComponentTypeInfo() override { return s_componentTypeInfo; }

		virtual nlohmann::json serialize() override;
		virtual void deserialize(const nlohmann::json& j) override;

	private:
		static inline ComponentTypeInfo s_componentTypeInfo = { "Mesh Renderer", ComponentCategory::GRAPHICS, true };
	};
}