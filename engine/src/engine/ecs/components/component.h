#pragma once
#include "engine/ecs/core/entity.h"
#include "utils/text/fixedString.h"
#include "utils/text/stringUtils.h"

namespace emerald {
	enum class ComponentCategory {
		INFO,
		GRAPHICS,
		PHYSICS,
		SCRIPTING,
		OTHER,
		INTERNAL, //Not drawn in inspector
		_COUNT
	};

	struct ComponentTypeInfo {
		FixedString<64> name;
		ComponentCategory category;
		uint32_t inspectorPriorityIndex;
		mutable bool collapsedInInspector = false;
		bool canBeDisabled = true;

		ComponentTypeInfo(const FixedString<64>& _name, ComponentCategory _category, bool _canBeDisabled)
			: name(_name), category(_category), inspectorPriorityIndex(calculateInspectorPriorityIndex(_name)), canBeDisabled(_canBeDisabled) {
		}

	private:
		static uint32_t calculateInspectorPriorityIndex(const FixedString<64>& componentName) {
			if (componentName.compare("Transform") == 0) {
				return 0;
			}

			uint32_t hash = StringUtils::hash32(componentName);

			return hash != 0 ? hash : 1;
		}
	};

	class Component : public RefCounted {
		RTTI_BASE_CLASS_DECL(Component);
	public:
		Component() = default;
		//virtual ~Component() = default;

		Entity m_entity = Entity();
		virtual const ComponentTypeInfo& getComponentTypeInfo() = 0;
		virtual nlohmann::json serialize() = 0;

		uint64_t getHash();

		friend class InspectorPanel;
	};
}