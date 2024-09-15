#pragma once
#include "component.h"
#include "rtti.h"
#include "ecs/entity.h"

namespace emerald {
	class NameComponent : public Component {
		RTTI_DERIVED_CLASS_DECL(NameComponent, Component);
	public:
		std::string m_name;

		NameComponent(const std::string& name)
			: Component(), m_name(name) {
		}
	};
}