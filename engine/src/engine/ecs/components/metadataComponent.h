#pragma once
#include "component.h"
#include "engine/ecs/core/entity.h"
#include "utils/misc/flags.h"

namespace emerald {
	static constexpr size_t NAME_MAX_LENGTH = 64;

	class MetadataComponent : public Component {
		RTTI_DERIVED_CLASS_DECL(MetadataComponent, Component);
	public:
		enum MetadataFlags {
			ENTITY_STATIC = BIT(0)
		};

		FixedString<NAME_MAX_LENGTH> m_name;

		MetadataComponent(const std::string& name)
			: Component(), m_name(name.c_str()) {

			setStatic(false);
		}

		MetadataComponent(const char* name)
			: Component(), m_name(name) {
		}

		void setStatic(bool isStatic) { m_flags.setFlag(ENTITY_STATIC, isStatic); }
		bool isStatic() const { return m_flags.isFlagSet(ENTITY_STATIC); }

		const ComponentTypeInfo& getComponentTypeInfo() override { return s_componentTypeInfo; }

	private:
		Flags8 m_flags;

		static inline ComponentTypeInfo s_componentTypeInfo = { "EntityInfo", ComponentCategory::INTERNAL, false };
	};
}