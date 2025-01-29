#pragma once
#include <string_view>
#include "utils/uuid/uuid.h"
#include "component.h"
#include "../core/ECSManager.h"
#include "engine/ecs/core/entityComponentSystem.h"

#define COMPONENT_DECL(_thisclass)														        \
RTTI_DERIVED_CLASS_DECL(_thisclass, Component)											        \
public:                                                                                         \
    static emerald::Component* addToECS(UUID entity, UUID componentID) {                        \
        Component* c = ECSManager::ECS().addComponentWithID<_thisclass>(entity, componentID);   \
        return c;                                                                               \
    }                                                                                           \
private:                                                                                        \
    static bool s_componentRegistered;

#define COMPONENT_DEF(_thisclass)														        \
RTTI_CLASS_DEF(_thisclass);																        \
bool _thisclass::s_componentRegistered =                                                        \
    emerald::ComponentRegistry::                                                                \
        registerComponent(std::type_index(typeid(_thisclass)),                                  \
                           &_thisclass::addToECS)

namespace emerald {
	class Component;
	class ComponentRegistry {
    public:
		using AddDeserializeFunc = Component* (*)(UUID, UUID);

		static bool registerComponent(RTTIType type, AddDeserializeFunc func);
		static Component* create(RTTIType type, UUID entity, UUID componentID);

	private:
		static inline std::unordered_map<RTTIType, AddDeserializeFunc> s_factories;
	};
}