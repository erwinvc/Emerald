#pragma once
#include "rtti.h"
#include "ref.h"
#include "ecs/entity.h"
#include "fixedString.h"

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
		uint32_t priorityIndex;
		mutable bool collapsedInInspector = false;

		ComponentTypeInfo(const FixedString<64>& _name, ComponentCategory _category)
			: name(_name), category(_category), priorityIndex(calculatePriorityIndex(_name)) {
		}

	private:
		static uint32_t calculatePriorityIndex(const FixedString<64>& componentName) {
			if (componentName.compare("Transform") == 0) {
				return 0;
			}

			// DJB2 hash algorithm
			uint32_t hash = 5381;
			for (size_t i = 0; i < componentName.size(); ++i) {
				hash = ((hash << 5) + hash) + static_cast<uint32_t>(componentName[i]);
			}

			return hash != 0 ? hash : 1;
		}
	};

	class Component : public RefCounted {
		RTTI_BASE_CLASS_DECL(Component);
	public:
		Component() = default;
		bool m_enabled = true;
		Entity m_entity = Entity();
		virtual const ComponentTypeInfo& getComponentTypeInfo() = 0;
	};
}