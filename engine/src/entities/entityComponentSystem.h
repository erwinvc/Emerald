#pragma once
#include "components.h"
#include "componentArray.h"
#include "util/utils.h"

//RTTI based Entity Component System
namespace emerald {
	class EntityComponentSystem {
	public:
		template <typename T>
		void registerComponent() {
			ASSERT_RTTI(T);
			RTTIType type = T::getStaticClassType();
			isComponent<T>();
			ASSERT(m_componentArrays.find(type) == m_componentArrays.end(), "Registering component type more than once");
			m_componentArrays[type] = std::make_shared<ComponentArray<T>>();
		}

		uint32_t getFreeEntityIndex();
		uint32_t createEntityFromID(uint32_t ID, const std::string& name, bool isRootEntity = false);
		uint32_t createEntity(const std::string& name, bool isRootEntity = false);

		void destroyEntity(uint32_t entity) {
			//Recursively destroy children
			auto* sgc = getComponent<SceneGraphComponent>(entity);
			if (sgc) {
				auto children = sgc->getChildren();
				for (auto* child : children) {
					destroyEntity(child->m_entity);
				}
			}
			utils::eraseFromVector(m_entities, entity);

			for (auto& pair : m_componentArrays) {
				pair.second->removeComponent(entity);
			}
		}

		template<typename T, typename... Args>
		WeakRef<T> addComponent(uint32_t entity, Args&&... args) {
			if (!entity) return nullptr;
			static_assert(std::is_base_of<Component, T>::value, "T must be derived from Component");
			WeakRef<T> component = getComponentArray<T>().insert((uint32_t)entity, std::forward<Args>(args)...);
			component.lock()->m_entity = entity;
			return component;
		}

		//void addComponent(uint32_t entity, Component* component) {
		//	getComponentArray(component->getClassType()).insert(entity, component);
		//}

		template <typename T>
		void removeComponent(uint32_t entity) {
			if (!entity) return;
			ASSERT_RTTI(T);
			if (!isRemovableComponent(T::getStaticClassType())) return;
			getComponentArray<T>()->removeComponent(entity);
		}

		template <typename T>
		T* getComponent(uint32_t entity) {
			if (!entity) return nullptr;
			return getComponentArray<T>().get(entity);
		}

		template <typename T>
		ComponentArray<T>& getComponentArray() {
			RTTIType type = T::getStaticClassType();
			ASSERT(m_componentArrays.find(type) != m_componentArrays.end(), "Component not registered before use");
			return (ComponentArray<T>&)*m_componentArrays[type].get();
		}

		ComponentArray<Component>& getComponentArray(RTTIType type) {
			ASSERT(m_componentArrays.find(type) != m_componentArrays.end(), "Component not registered before use");
			return (ComponentArray<Component>&) * m_componentArrays[type].get();
		}

		template <typename T>
		bool hasComponent(uint32_t entity) {
			if (!entity) return false;
			return getComponentArray<T>()->has(entity);
		}

		std::vector<uint32_t>& getEntities() {
			return m_entities;
		}

	private:
		std::unordered_map<RTTIType, std::shared_ptr<ComponentArrayBase>> m_componentArrays;
		uint32_t m_nextEntityID = 1; //Entity 0 is reserved for null entity
		std::vector<uint32_t> m_entities;

		template <typename T>
		bool isComponent() {
			ASSERT_RTTI(T);
			return T::isClassType(Component::getStaticClassType());
		}
		RTTIType getComponentRTTIType() const;
		bool isRemovableComponent(RTTIType type) const;
	};
}