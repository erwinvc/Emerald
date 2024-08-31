#pragma once
#include "components.h"
#include "componentArray.h"
#include "util/utils.h"

namespace emerald {
	class EntityComponentSystem {
	public:
		template <typename T>
		void registerComponent() {
			size_t typeID = getComponentTypeID<T>();
			ASSERT(m_componentArrays.find(typeID) == m_componentArrays.end(), "Registering component type more than once");
			m_componentArrays[typeID] = std::make_shared<ComponentArray<T>>();
		}

		uint32_t createEntity(const std::string& name);

		void destroyEntity(uint32_t entity) {

			for (auto& pair : m_componentArrays) {
				pair.second->removeComponent(entity);
			}

			utils::eraseFromVector(m_entities, entity);
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
			static_assert(!std::is_base_of<UUIDComponent, T>::value, "UUIDComponents can not be removed from entities");
			static_assert(!std::is_base_of<NameComponent, T>::value, "NameComponents can not be removed from entities");
			static_assert(!std::is_base_of<TransformComponent, T>::value, "TransformComponents can not be removed from entities");
			static_assert(!std::is_base_of<SceneGraphComponent, T>::value, "SceneGraphComponents can not be removed from entities");

			getComponentArray<T>()->removeComponent(entity);
		}

		template <typename T>
		T& getComponent(uint32_t entity) {
			return getComponentArray<T>()->get(entity);
		}

		template <typename T>
		std::shared_ptr<ComponentArray<T>> getComponentArray() {
			size_t typeID = getComponentTypeID<T>();
			ASSERT(m_componentArrays.find(typeID) != m_componentArrays.end(), "Component not registered before use");
			return std::static_pointer_cast<ComponentArray<T>>(m_componentArrays[typeID]);
		}

		template <typename T>
		bool hasComponent(uint32_t entity) {
			return getComponentArray<T>()->has(entity);
		}

		std::vector<uint32_t>& getEntities() {
			return m_entities;
		}

	private:
		std::unordered_map<size_t, std::shared_ptr<ComponentArrayBase>> m_componentArrays;
		size_t m_nextComponentType = 0;
		uint32_t m_nextEntityID = 0;
		std::vector<uint32_t> m_entities;
		
		template <typename T>
		size_t getComponentTypeID() noexcept {
			static const size_t typeID = m_nextComponentType++;
			return typeID;
		}
	};
}