#include "eepch.h"
#include "componentRegistry.h"

namespace emerald {
	bool ComponentRegistry::registerComponent(RTTIType type, AddDeserializeFunc func) {
		auto [it, inserted] = s_factories.emplace(type, func);
		return inserted;
	}

	Component* ComponentRegistry::create(RTTIType type, UUID entity, UUID componentID) {
		auto it = s_factories.find(type);
		if (it != s_factories.end()) {
			Component* comp = it->second(entity, componentID);
			return comp;
		}
		return nullptr;
	}
}