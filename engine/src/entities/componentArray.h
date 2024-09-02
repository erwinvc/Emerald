#pragma once
#include "ref.h"

namespace emerald {
	class ComponentArrayBase {
	public:
		friend class EntityComponentSystem;
		virtual ~ComponentArrayBase() = default;

	private:
		virtual void removeComponent(uint32_t entity) = 0;
	};

	template <typename T>
	class ComponentArray : public ComponentArrayBase {
	public:
		friend class EntityComponentSystem;
		virtual ~ComponentArray() override {}

		T* get(uint32_t entity) {
			if (!entity) return nullptr;
			ASSERT(entityToIndexMap.find(entity) != entityToIndexMap.end(), "Retrieving non-existent component");
			return components[entityToIndexMap[entity]].raw();
		}

		bool has(uint32_t entity) {
			return entityToIndexMap.find(entity) != entityToIndexMap.end();
		}

		std::vector<UniqueRef<T>>& getVector() {
			return components;
		}

		auto begin() {
			return components.begin();
		}

		auto end() {
			return components.end();
		}

		auto begin() const {
			return components.begin();
		}

		auto end() const {
			return components.end();
		}

	private:
		std::vector<UniqueRef<T>> components; 
		std::unordered_map<uint32_t, size_t> entityToIndexMap;
		std::unordered_map<size_t, uint32_t> indexToEntityMap;

		template<typename... Args>
		T* insert(uint32_t entity, Args&&... args) {
			if (!entity) return nullptr;
			assert(entityToIndexMap.find(entity) == entityToIndexMap.end() && "Component added to the same entity more than once");
			size_t newIndex = components.size();
			components.emplace_back(UniqueRef<T>::create(std::forward<Args>(args)...));
			entityToIndexMap[entity] = newIndex;
			indexToEntityMap[newIndex] = entity;
			return components.back().raw();
		}

		void removeComponent(uint32_t entity) override {
			if (!entity) return;
			ASSERT(entityToIndexMap.find(entity) != entityToIndexMap.end(), "Removing non-existent component");
			size_t indexToRemove = entityToIndexMap[entity];
			size_t lastIndex = components.size() - 1;

			components[indexToRemove] = std::move(components[lastIndex]);
			uint32_t lastEntity = indexToEntityMap[lastIndex];

			entityToIndexMap[lastEntity] = indexToRemove;
			indexToEntityMap[indexToRemove] = lastEntity;

			components.pop_back();

			entityToIndexMap.erase(entity);
			indexToEntityMap.erase(lastIndex);
		}
	};
}