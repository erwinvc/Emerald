#pragma once
#include "component.h"
#include "rtti.h"
#include "ecs/entity.h"
#include "assets/mesh.h"

namespace emerald {
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