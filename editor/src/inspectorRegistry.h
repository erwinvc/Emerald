#pragma once
#include <functional>
#include "rtti.h"
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
			getInstance().m_registry[componentType] = std::move(inspector);
		}

		static InspectorFunction getInspector(RTTIType componentType) {
			auto it = getInstance().m_registry.find(componentType);
			if (it != getInstance().m_registry.end()) {
				return it->second;
			}
			return DefaultComponentInspector::draw;
		}

	private:
		std::unordered_map<RTTIType, InspectorFunction> m_registry;

		static InspectorRegistry& getInstance() {
			static InspectorRegistry instance;
			return instance;
		}
	};
}