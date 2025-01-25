#pragma once
#include "utils/uuid/uuid.h"
#include <vector>
#include "componentChunk.h"

namespace emerald {
	class Component;

	class ComponentArrayBase {
	public:
		virtual ~ComponentArrayBase() = default;

		virtual void setEntityEnabled(UUID entity, bool enabled) = 0;
		virtual void setEnabled(UUID entity, bool enabled) = 0;

		virtual void removeComponentsOfEntity(UUID entity) = 0;

		virtual std::vector<Component*> getComponentsAsBase(UUID entity) = 0;

		virtual RTTIType getComponentType() const = 0;

		virtual void clear() = 0;
	};

	template<typename T, size_t ChunkSize = 256>
	class ComponentArray : public ComponentArrayBase {
	public:
		ComponentArray() = default;
		~ComponentArray() {
			clear();
		}

		template<typename... Args>
		T* addComponent(UUID entity, Args&&... args) {
			ComponentIndex index = findFreeSlot();
			T* componentPtr = new (&m_chunks[index.chunkIndex]->getComponentsArray()[index.componentIndex]) T(std::forward<Args>(args)...);
			m_chunks[index.chunkIndex]->setEntityEnabled(index.componentIndex, true);
			m_chunks[index.chunkIndex]->setEnabled(index.componentIndex, true);
			m_chunks[index.chunkIndex]->count++;
			m_entityToComponentIndices[entity].push_back(index);
			m_componentToEntity[ComponentKey{ index.chunkIndex, index.componentIndex }] = entity;
			componentPtr->m_entity = entity;
			return componentPtr;
		}

		void removeComponent(UUID entity, T* componentPtr) {
			auto it = m_entityToComponentIndices.find(entity);
			if (it == m_entityToComponentIndices.end()) return;

			auto& indices = it->second;
			for (auto indexIt = indices.begin(); indexIt != indices.end(); ++indexIt) {
				T* storedComponentPtr = m_chunks[indexIt->chunkIndex]->getComponent(indexIt->componentIndex);
				if (storedComponentPtr == componentPtr) {
					m_chunks[indexIt->chunkIndex]->removeComponent(indexIt->componentIndex);
					indices.erase(indexIt);
					if (indices.empty()) {
						m_entityToComponentIndices.erase(it);
					}
					return;
				}
			}
		}

		void removeComponentsOfEntity(UUID entity) override {
			auto it = m_entityToComponentIndices.find(entity);
			if (it == m_entityToComponentIndices.end()) return;

			auto& indices = it->second;
			for (const auto& index : indices) {
				T* componentPtr = m_chunks[index.chunkIndex]->getComponent(index.componentIndex);
				componentPtr->~T();
				m_freeSlots.push_back(index);
				m_chunks[index.chunkIndex]->count--;
				m_componentToEntity.erase(ComponentKey{ index.chunkIndex, index.componentIndex });
			}
			m_entityToComponentIndices.erase(it);
		}

		std::vector<T*> getComponents(UUID entity) {
			std::vector<T*> components;
			auto it = m_entityToComponentIndices.find(entity);
			if (it != m_entityToComponentIndices.end()) {
				for (const auto& index : it->second) {
					T* componentPtr = m_chunks[index.chunkIndex]->getComponent(index.componentIndex);
					components.push_back(componentPtr);
				}
			}
			return components;
		}

		T* getComponent(UUID entity) {
			auto a = std::type_index(typeid(T));
			auto it = m_entityToComponentIndices.find(entity);
			if (it != m_entityToComponentIndices.end()) {
				for (const auto& index : it->second) {
					return m_chunks[index.chunkIndex]->getComponent(index.componentIndex);
				}
			}
			return nullptr;
		}

		std::vector<T*> getAllComponents() {
			std::vector<T*> components;
			for (auto& chunk : m_chunks) {
				T* chunkArray = chunk->getComponentsArray();
				for (size_t i = 0; i < chunk->getCount(); ++i) {
					components.push_back(&chunkArray[i]);
				}
			}
			return components;
		}

		std::vector<Component*> getComponentsAsBase(UUID entity) override {
			std::vector<Component*> components;
			auto it = m_entityToComponentIndices.find(entity);
			if (it != m_entityToComponentIndices.end()) {
				for (const auto& index : it->second) {
					Component* componentPtr = m_chunks[index.chunkIndex]->getComponent(index.componentIndex);
					components.push_back(componentPtr);
				}
			}
			return components;
		}

		RTTIType getComponentType() const override {
			return std::type_index(typeid(T));
		}

		void setEnabled(UUID entity, bool enabled) override {
			auto it = m_entityToComponentIndices.find(entity);
			if (it != m_entityToComponentIndices.end()) {
				for (const auto& index : it->second) {
					m_chunks[index.chunkIndex]->setEnabled(index.componentIndex, enabled);
				}
			}
		}

		void setEntityEnabled(UUID entity, bool enabled) override {
			auto it = m_entityToComponentIndices.find(entity);
			if (it != m_entityToComponentIndices.end()) {
				for (const auto& index : it->second) {
					m_chunks[index.chunkIndex]->setEntityEnabled(index.componentIndex, enabled);
				}
			}
		}

		T* getComponentInChunk(size_t chunkIndex, size_t componentIndex) {
			return m_chunks[chunkIndex]->getComponent(componentIndex);
		}

		UUID getEntityInChunk(size_t chunkIndex, size_t componentIndex) {
			ComponentKey key{ chunkIndex, componentIndex };
			auto it = m_componentToEntity.find(key);
			if (it != m_componentToEntity.end()) {
				return it->second;
			}
			return UUID(); // Return an invalid UUID if not found
		}

		size_t getChunkCount() const {
			return m_chunks.size();
		}

		ComponentChunk<T, ChunkSize>* getChunk(size_t chunkIndex) {
			return m_chunks[chunkIndex].raw();
		}

		virtual void clear() override {
			for (auto& chunk : m_chunks) {
				T* chunkArray = chunk->getComponentsArray();
				for (size_t i = 0; i < chunk->getCount(); ++i) {
					chunkArray[i].~T();
				}
			}
			m_chunks.clear();
			m_entityToComponentIndices.clear();
			m_freeSlots.clear();
		}

	private:
		struct ComponentIndex {
			size_t chunkIndex;
			size_t componentIndex;
		};

		struct ComponentKey {
			size_t chunkIndex;
			size_t componentIndex;

			bool operator==(const ComponentKey& other) const {
				return chunkIndex == other.chunkIndex && componentIndex == other.componentIndex;
			}
		};

		struct ComponentKeyHash {
			std::size_t operator()(const ComponentKey& key) const {
				return std::hash<size_t>()(key.chunkIndex) ^ (std::hash<size_t>()(key.componentIndex) << 1);
			}
		};

		ComponentIndex findFreeSlot() {
			if (!m_freeSlots.empty()) {
				ComponentIndex index = m_freeSlots.back();
				m_freeSlots.pop_back();
				return index;
			} else {
				if (m_chunks.empty() || m_chunks.back()->isFull()) {
					m_chunks.emplace_back(UniqueRef<ComponentChunk<T, ChunkSize>>::create());
				}
				size_t chunkIndex = m_chunks.size() - 1;
				size_t componentIndex = m_chunks[chunkIndex]->getCount();
				return { chunkIndex, componentIndex };
			}
		}

		std::vector<UniqueRef<ComponentChunk<T, ChunkSize>>> m_chunks;
		std::unordered_map<UUID, std::vector<ComponentIndex>> m_entityToComponentIndices;
		std::unordered_map<ComponentKey, UUID, ComponentKeyHash> m_componentToEntity;

		std::vector<ComponentIndex> m_freeSlots;
	};

}