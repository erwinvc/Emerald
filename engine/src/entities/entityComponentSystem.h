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
			RTTIType type = T::getClassType();
			isComponent<T>();
			ASSERT(m_componentArrays.find(type) == m_componentArrays.end(), "Registering component type more than once");
			m_componentArrays[type] = std::make_shared<ComponentArray<T>>();
		}

		uint32_t createEntity(const std::string& name);

		void destroyEntity(uint32_t entity) {
			auto& sgc = getComponent<SceneGraphComponent>(entity);
			utils::eraseFromVector(m_sceneGraph, &sgc);
			utils::eraseFromVector(m_entities, entity);

			for (auto& pair : m_componentArrays) {
				pair.second->removeComponent(entity);
			}
		}

		template<typename T, typename... Args>
		T& addComponent(uint32_t entity, Args&&... args) {
			static_assert(std::is_base_of<Component, T>::value, "T must be derived from Component");
			T& component = getComponentArray<T>()->insert((uint32_t)entity, std::forward<Args>(args)...);
			component.m_entity = entity;
			return component;
		}

		template <typename T>
		void removeComponent(uint32_t entity) {
			ASSERT_RTTI(T);
			if (!isRemovableComponent(T::getStaticClassType())) return;
			getComponentArray<T>()->removeComponent(entity);
		}

		template <typename T>
		T& getComponent(uint32_t entity) {
			return getComponentArray<T>()->get(entity);
		}

		template <typename T>
		std::shared_ptr<ComponentArray<T>> getComponentArray() {
			RTTIType type = T::getClassType();
			ASSERT(m_componentArrays.find(type) != m_componentArrays.end(), "Component not registered before use");
			return std::static_pointer_cast<ComponentArray<T>>(m_componentArrays[type]);
		}

		template <typename T>
		bool hasComponent(uint32_t entity) {
			return getComponentArray<T>()->has(entity);
		}

		std::vector<uint32_t>& getEntities() {
			return m_entities;
		}

		std::vector<SceneGraphComponent*>& getSceneGraph() {
			return m_sceneGraph;
		}

	private:
		std::unordered_map<RTTIType, std::shared_ptr<ComponentArrayBase>> m_componentArrays;
		uint32_t m_nextEntityID = 0;
		std::vector<uint32_t> m_entities;
		std::vector<SceneGraphComponent*> m_sceneGraph;

		template <typename T>
		bool isComponent() {
			ASSERT_RTTI(T);
			return T::isClassType(Component::getClassType());
		}
		RTTIType getComponentRTTIType() const;
		bool isRemovableComponent(RTTIType type) const;
	};
}