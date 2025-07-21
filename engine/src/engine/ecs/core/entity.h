#pragma once
#include "utils/uuid/uuid.h"
#include "ECSManager.h"

namespace emerald {
	class SceneManager;

	class Entity {
	public:
		Entity() : m_id() {}
		Entity(UUID id) : m_id(id) {}

		operator UUID() const { return m_id; }

		static Entity create(const std::string& name);

		operator bool() { return m_id.isValid(); }
		operator bool() const { return m_id.isValid(); }
		bool operator==(const Entity& other) const { return m_id == other.m_id; }
		bool operator!=(const Entity& other) const { return !(*this == other); }

		template <typename T>
		void addComponent(const T& component) {
			//ECSManager::ECS().addComponent<T>(*this, component);
			ECSManager::ECS().template addComponent<T>(*this, component);
		}

		template <typename T>
		void removeComponent() {
			ECSManager::ECS().template removeComponent<T>(*this);
		}

		template <typename T>
		std::vector<T*> getComponents() {
			return ECSManager::ECS().template getComponents<T>(*this);
		}

		template <typename T>
		T* getComponent() {
			return ECSManager::ECS().template getComponent<T>(*this);
		}

		template <typename T>
		bool hasComponent() {
			return ECSManager::ECS().template hasComponent<T>(*this);
		}

	private:
		UUID m_id;
	};
}