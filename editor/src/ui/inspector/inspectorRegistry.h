#pragma once
#include <functional>
#include "ui/inspector/componentInspector.h"

#define REGISTER_COMPONENT_INSPECTOR(ComponentClass, InspectorClass)							\
namespace {																						\
    struct InspectorRegistrar_##ComponentClass {												\
        InspectorRegistrar_##ComponentClass() {													\
            InspectorRegistry::registerInspector(typeid(ComponentClass), InspectorClass::draw);	\
        }																						\
    } inspectorRegistrar_##ComponentClass;														\
}

namespace emerald {
	using InspectorFunction = std::function<void(const std::vector<Component*>&)>;
	class InspectorRegistry {
	public:
		using InspectorFactory = std::function<ComponentInspector* ()>;

		static void registerInspector(RTTIType componentType, InspectorFunction inspector) {
			s_registry[componentType] = std::move(inspector);
		}

		static InspectorFunction getInspector(RTTIType componentType) {
			auto it = s_registry.find(componentType);
			if (it != s_registry.end()) {
				return it->second;
			}
			return DefaultComponentInspector::draw;
		}

	private:
		static inline std::unordered_map<RTTIType, InspectorFunction> s_registry;
	};
}