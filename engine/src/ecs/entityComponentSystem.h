#pragma once
#include "componentArray.h"
#include "util/utils.h"
#include "components/sceneGraphComponent.h"

//RTTI based Entity Component System
namespace emerald {
	class EntityComponentSystem {
	public:
		template<typename MainComponent, typename... OtherComponents>
		class View {
		public:
			View(EntityComponentSystem* ecs)
				: m_ecs(ecs), m_mainArray(ecs->getComponentArray<MainComponent>().getVector()) {
			}

			class iterator {
			public:
				iterator(EntityComponentSystem* ecs, const std::vector<Ref<MainComponent>>& mainArray, size_t index)
					: m_ecs(ecs), m_mainArray(mainArray), m_index(index) {
				}

				iterator& operator++() {
					++m_index;
					return *this;
				}

				bool operator!=(const iterator& other) const {
					return m_index != other.m_index;
				}

				auto operator*() {
					MainComponent* mainComp = m_mainArray[m_index].raw();
					UUID entity = mainComp->m_entity;
					return std::tuple<MainComponent*, OtherComponents*...>(mainComp, m_ecs->getComponent<OtherComponents>(entity)...);
				}

			private:
				EntityComponentSystem* m_ecs;
				const std::vector<Ref<MainComponent>>& m_mainArray;
				size_t m_index;
			};

			iterator begin() {
				return iterator(m_ecs, m_mainArray, 0);
			}

			iterator end() {
				return iterator(m_ecs, m_mainArray, m_mainArray.size());
			}

		private:
			EntityComponentSystem* m_ecs;
			const std::vector<Ref<MainComponent>>& m_mainArray;
		};

		template<typename MainComponent, typename... OtherComponents>
		View<MainComponent, OtherComponents...> view() {
			return View<MainComponent, OtherComponents...>(this);
		}

		template <typename T>
		void registerComponent() {
			ASSERT_RTTI(T);
			RTTIType type = T::getStaticClassType();
			isComponent<T>();
			ASSERT(m_componentArrays.find(type) == m_componentArrays.end(), "Registering component type more than once");
			m_componentArrays[type] = std::make_shared<ComponentArray<T>>();
		}

		UUID getNewEntityID();
		UUID createEntityFromID(UUID ID, const std::string& name, bool isRootEntity = false);
		UUID createEntity(const std::string& name, bool isRootEntity = false);

		void destroyEntity(UUID entity) {
			//Recursively destroy children
			auto* sgc = getComponent<SceneGraphComponent>(entity);
			if (sgc) {
				auto& children = sgc->getChildren();
				for (SceneGraphComponent* child : children) {
					destroyEntity(child->m_entity);
				}
			}
			utils::eraseFromVector(m_entities, entity);

			for (auto& pair : m_componentArrays) {
				pair.second->removeComponent(entity);
			}
		}

		template<typename T, typename... Args>
		WeakRef<T> addComponent(UUID entity, Args&&... args) {
			if (!entity) return nullptr;
			static_assert(std::is_base_of<Component, T>::value, "T must be derived from Component");
			const WeakRef<T>& component = getComponentArray<T>().insert(entity, std::forward<Args>(args)...);
			component.lock()->m_entity = entity;
			return component;
		}

		//void addComponent(uint32_t entity, Component* component) {
		//	getComponentArray(component->getClassType()).insert(entity, component);
		//}

		template <typename T>
		void removeComponent(UUID entity) {
			if (!entity) return;
			ASSERT_RTTI(T);
			if (!isRemovableComponent(T::getStaticClassType())) return;
			getComponentArray<T>()->removeComponent(entity);
		}

		template <typename T>
		T* getComponent(UUID entity) {
			if (!entity) return nullptr;
			auto& componentArray = getComponentArray<T>();
			return componentArray.get(entity);
		}

		template<typename T>
		std::vector<T*> getComponents(const std::vector<UUID>& entities) {
			std::vector<T*> components;
			auto& componentArray = getComponentArray<T>();
			for (UUID entity : entities) {
				if (componentArray.hasComponent(entity)) {
					components.push_back(componentArray.get(entity));
				}
			}
			return components;
		}

		template <typename T>
		ComponentArray<T>& getComponentArray() const {
			RTTIType type = T::getStaticClassType();
			auto it = m_componentArrays.find(type);  // Use find() instead of operator[]
			ASSERT(it != m_componentArrays.end(), "Component not registered before use");
			return (ComponentArray<T>&) * it->second;  // Access the value through the iterator
		}

		ComponentArray<Component>& getComponentArray(RTTIType type) {
			ASSERT(m_componentArrays.find(type) != m_componentArrays.end(), "Component not registered before use");
			return (ComponentArray<Component>&) * m_componentArrays[type].get();
		}

		template <typename T>
		bool hasComponent(UUID entity) const {
			if (!entity) return false;
			const auto& componentArray = getComponentArray<T>();
			return componentArray.has(entity);
		}

		const std::unordered_map<RTTIType, std::shared_ptr<ComponentArrayBase>> getComponentArrays() const {
			return m_componentArrays;
		}

		std::vector<UUID>& getEntities() {
			return m_entities;
		}

	private:
		std::unordered_map<RTTIType, std::shared_ptr<ComponentArrayBase>> m_componentArrays;
		UUID m_nextEntityID; //Entity 0 is reserved for null entity
		std::vector<UUID> m_entities;

		template <typename T>
		bool isComponent() {
			ASSERT_RTTI(T);
			return T::isClassType(Component::getStaticClassType());
		}
		RTTIType getComponentRTTIType() const;
		bool isRemovableComponent(RTTIType type) const;
	};
}