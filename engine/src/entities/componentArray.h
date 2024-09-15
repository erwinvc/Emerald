#pragma once
#include "ref.h"
#include "util/uuid.h"

namespace emerald {
	class Component;

	class ComponentArrayBase {
	public:
		friend class EntityComponentSystem;
		virtual ~ComponentArrayBase() = default;

		virtual Component* getRaw(UUID entity) = 0;
		virtual bool has(UUID entity) const = 0;

	private:
		virtual void removeComponent(UUID entity) = 0;
	};

	template <typename T>
	class ComponentArray : public ComponentArrayBase {
	public:
		friend class EntityComponentSystem;
		virtual ~ComponentArray() override {}

		T* get(UUID entity) {
			if (!entity) return nullptr;
			ASSERT(entityToIndexMap.find(entity) != entityToIndexMap.end(), "Retrieving non-existent component");
			return components[entityToIndexMap[entity]].raw();
		}

		Component* getRaw(UUID entity) override {
			if (!entity) return nullptr;
			ASSERT(entityToIndexMap.find(entity) != entityToIndexMap.end(), "Retrieving non-existent component");
			return components[entityToIndexMap[entity]].raw();
		}

		bool has(UUID entity) const {
			return entityToIndexMap.find(entity) != entityToIndexMap.end();
		}

		std::vector<Ref<T>>& getVector() {
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
		std::vector<Ref<T>> components;
		std::unordered_map<UUID, size_t> entityToIndexMap;
		std::unordered_map<size_t, UUID> indexToEntityMap;

		template<typename... Args>
		WeakRef<T> insert(UUID entity, Args&&... args) {
			if (!entity) return nullptr;
			assert(entityToIndexMap.find(entity) == entityToIndexMap.end() && "Component added to the same entity more than once");
			size_t newIndex = components.size();
			components.emplace_back(Ref<T>::create(std::forward<Args>(args)...));
			entityToIndexMap[entity] = newIndex;
			indexToEntityMap[newIndex] = entity;
			return components.back();
		}

		void removeComponent(UUID entity) override {
			if (!entity) return;
			if (!entityToIndexMap.contains(entity)) return;
			ASSERT(entityToIndexMap.find(entity) != entityToIndexMap.end(), "Removing non-existent component");
			size_t indexToRemove = entityToIndexMap[entity];
			size_t lastIndex = components.size() - 1;

			components[indexToRemove] = std::move(components[lastIndex]);
			UUID lastEntity = indexToEntityMap[lastIndex];

			entityToIndexMap[lastEntity] = indexToRemove;
			indexToEntityMap[indexToRemove] = lastEntity;

			components.pop_back();

			entityToIndexMap.erase(entity);
			indexToEntityMap.erase(lastIndex);
		}
	};
}