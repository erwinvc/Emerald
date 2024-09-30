#pragma once
#include "component.h"
#include "rtti.h"
#include "ecs/entity.h"
#include "../../fixedString.h"

namespace emerald {
	static constexpr size_t NAME_MAX_LENGTH = 64;

	class NameComponent : public Component {
		RTTI_DERIVED_CLASS_DECL(NameComponent, Component);
	public:
		FixedString<NAME_MAX_LENGTH> m_name;

		NameComponent(const std::string& name)
			: Component(), m_name(name.c_str()) {
		}

		NameComponent(const char* name)
			: Component(), m_name(name) {
		}

		const ComponentTypeInfo& getComponentTypeInfo() override { return s_componentTypeInfo; }

	private:
		static inline ComponentTypeInfo s_componentTypeInfo = { "Name", ComponentCategory::INTERNAL };
	};
}