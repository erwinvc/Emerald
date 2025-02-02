#pragma once
#include "ECSManager.h"
#include "engine/ecs/components/component.h"
#include "engine/ecs/internal/componentArray.h"
#include "utils/misc/flags.h"
#include "utils/uuid/uuid.h"
#include <memory>
#include <typeindex>
#include <unordered_map>

namespace emerald {
	enum EntityFlags {
		ENTITY_ENABLED = BIT(0),
		ENTITY_PERSISTENT = BIT(1), //Unused for now
		ENTITY_PENDING_REMOVAL = BIT(2) //Unused for now
	};

	class EntityComponentSystem {
	public:
		template<typename MainComponent, typename... OtherComponents>
		class View {
		public:
			View(EntityComponentSystem* ecs = &ECSManager::ECS())
				: m_ecs(ecs) {
				m_componentArray = ecs->getComponentArray<MainComponent>();
			}

			class iterator {
			public:
				iterator(EntityComponentSystem* ecs, ComponentArray<MainComponent>* componentArray,
					size_t chunkIndex, size_t componentIndex)
					: m_ecs(ecs), m_componentArray(componentArray),
					m_chunkIndex(chunkIndex), m_componentIndex(componentIndex) {
					advanceToNextValid();
				}

				iterator& operator++() {
					if (m_componentArray) {
						++m_componentIndex;
						advanceToNextValid();
					}
					return *this;
				}

				bool operator!=(const iterator& other) const {
					if (m_componentArray == nullptr && other.m_componentArray == nullptr) {
						return false;
					}
					return m_chunkIndex != other.m_chunkIndex || m_componentIndex != other.m_componentIndex;
				}

				auto operator*() {
					if (!m_componentArray) {
						// Return a dummy tuple or handle it by assertion
						static MainComponent dummyMain;
						return std::tuple<MainComponent*, OtherComponents*...>(&dummyMain, (m_ecs->template getComponent<OtherComponents>(UUID()))...);
					}

					MainComponent* mainComp = m_componentArray->getComponentInChunk(m_chunkIndex, m_componentIndex);
					UUID entity = m_componentArray->getEntityInChunk(m_chunkIndex, m_componentIndex);

					return std::tuple<MainComponent*, OtherComponents*...>(mainComp, m_ecs->getComponent<OtherComponents>(entity)...);
				}

			private:
				void advanceToNextValid() {
					while (m_componentArray && m_chunkIndex < m_componentArray->getChunkCount()) {
						auto* chunk = m_componentArray->getChunk(m_chunkIndex);
						while (m_componentIndex < chunk->getCount()) {
							if (chunk->isEnabled(m_componentIndex)) {
								return;
							}
							++m_componentIndex;
						}
						++m_chunkIndex;
						m_componentIndex = 0;
					}
				}

				EntityComponentSystem* m_ecs;
				ComponentArray<MainComponent>* m_componentArray;
				size_t m_chunkIndex;
				size_t m_componentIndex;
			};

			iterator begin() {
				if (!m_componentArray) {
					return end();
				}
				return iterator(m_ecs, m_componentArray, 0, 0);
			}

			iterator end() {
				if (!m_componentArray) {
					return iterator(m_ecs, nullptr, 0, 0);
				}
				return iterator(m_ecs, m_componentArray, m_componentArray->getChunkCount(), 0);
			}

		private:
			EntityComponentSystem* m_ecs;
			ComponentArray<MainComponent>* m_componentArray;
		};


		UUID getNewEntityID();
		UUID createEntityFromID(UUID ID);
		UUID createEntityFromID(UUID ID, const std::string& name, bool isRootEntity = false);
		UUID createEntity(const std::string& name, bool isRootEntity = false);
		UUID createEntity();

		void destroyEntity(UUID entity) {
			auto it = m_entities.begin();
			while (it != m_entities.end()) {
				if (*it == entity) {
					it = m_entities.erase(it);
				} else {
					++it;
				}
			}

			for (auto& [typeIndex, componentArray] : m_componentArrays) {
				componentArray->removeComponentsOfEntity(entity);
			}
		}

		template<typename T, typename... Args>
		T* addComponent(UUID entity, Args&&... args) {
			markSceneDirty();
			if (!entity.isValid()) return nullptr;
			auto& componentArray = getOrCreateComponentArray<T>();
			return componentArray.addComponent(entity, std::forward<Args>(args)...);
		}

		template<typename T, typename... Args>
		T* addComponentWithID(UUID entity, UUID componentID, Args&&... args) {
			if (!entity.isValid()) return nullptr;
			auto& componentArray = getOrCreateComponentArray<T>();
			return componentArray.addComponentWithID(entity, componentID, std::forward<Args>(args)...);
		}

		template<typename T>
		void removeComponent(UUID entity, T* componentPtr) {
			markSceneDirty();
			if (!entity.isValid()) return;
			auto componentArray = getComponentArray<T>();
			if (componentArray) {
				componentArray->removeComponent(entity, componentPtr);
			}
		}

		template<typename T>
		std::vector<T*> getComponents(UUID entity) {
			auto componentArray = getComponentArray<T>();
			if (componentArray) {
				return componentArray->getComponents(entity);
			}
			return {};
		}

		template<typename T>
		T* getComponentByID(UUID componentID) {
			if(!componentID.isValid()) return nullptr;
			auto componentArray = getComponentArray<T>();
			if (componentArray) {
				return componentArray->getComponentByID(componentID);
			}
			return nullptr;
		}

		template<typename T>
		bool validateComponentID(UUID componentID) {
			auto componentArray = getComponentArray<T>();
			if (componentArray) {
				return componentArray->validateComponentID(componentID);
			}
		}

		std::vector<Component*> getAllComponents(UUID entity) {
			std::vector<Component*> components;
			for (auto& [typeIndex, componentArray] : m_componentArrays) {
				for (auto* component : componentArray->getComponentsAsBase(entity)) {
					components.push_back(component);
				}
			}
			return components;
		}

		template<typename T>
		T* getComponent(UUID entity) {
			auto componentArray = getComponentArray<T>();
			if (componentArray) {
				return componentArray->getComponent(entity);
			}
			return nullptr;
		}

		template <typename T>
		std::vector<T*> getComponentsInEntities(const std::vector<UUID>& entities) {
			std::vector<T*> components;
			auto componentArray = getComponentArray<T>();
			if (componentArray) {
				for (UUID entity : entities) {
					T* component = componentArray->getComponent(entity);
					if (component) components.push_back(component);
				}
			}
			return components;
		}

		template<typename T>
		std::vector<T*> getAllComponents() {
			auto componentArray = getComponentArray<T>();
			if (componentArray) {
				return componentArray->getAllComponents();
			}
			return {};
		}

		template<typename T>
		bool hasComponent(UUID entity) {
			auto componentArray = getComponentArray<T>();
			if (componentArray) {
				auto components = componentArray->getComponents(entity);
				return !components.empty();
			}
			return false;
		}

		ComponentArray<Component>& getComponentArray(RTTIType type) {
			ASSERT(m_componentArrays.find(type) != m_componentArrays.end(), "Component not registered before use");
			return (ComponentArray<Component>&) * m_componentArrays[type].get();
		}

		const std::vector<UUID>& getEntities() const {
			return m_entities;
		}

		const std::unordered_map<RTTIType, std::shared_ptr<ComponentArrayBase>> getComponentArrays() const {
			return m_componentArrays;
		}

		void setEntityEnabled(UUID entity, bool enabled) {
			m_entityFlags[entity].setFlag(ENTITY_ENABLED, enabled);
			for (auto& [typeIndex, componentArray] : m_componentArrays) {
				componentArray->setEntityEnabled(entity, enabled);
			}
		}

		bool isEntityEnabled(UUID entity) {
			return m_entityFlags[entity].isFlagSet(ENTITY_ENABLED);
		}

		void clear() {
			for (auto& [typeIndex, componentArray] : m_componentArrays) {
				componentArray->clear();
			}

			m_componentArrays.clear();
			m_entityFlags.clear();
			m_entities.clear();
		}

	private:
		template<typename T>
		ComponentArray<T>& getOrCreateComponentArray() {
			auto it = m_componentArrays.find(T::getStaticClassType());
			if (it == m_componentArrays.end()) {
				auto componentArray = std::make_shared<ComponentArray<T>>();
				m_componentArrays[T::getStaticClassType()] = componentArray;
				return *componentArray;
			}
			return *std::static_pointer_cast<ComponentArray<T>>(it->second);
		}

		template<typename T>
		ComponentArray<T>* getComponentArray() {
			auto it = m_componentArrays.find(T::getStaticClassType());
			if (it != m_componentArrays.end()) {
				return std::static_pointer_cast<ComponentArray<T>>(it->second).get();
			}
			return nullptr;
		}

		void markSceneDirty();

		std::unordered_map<std::type_index, std::shared_ptr<ComponentArrayBase>> m_componentArrays;
		std::unordered_map<UUID, Flags8> m_entityFlags;
		std::vector<UUID> m_entities;
		//uint64_t m_nextEntityID;
	};
}