#pragma once
#include "util/uuid.h"

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
			SceneManager::getActiveScene()->getECS().addComponent<T>(*this, component);
		}

		template <typename T>
		void removeComponent() {
			SceneManager::getActiveScene()->getECS().removeComponent<T>(*this);
		}

		template <typename T>
		T& getComponent() {
			return SceneManager::getActiveScene()->getECS().getComponent<T>(*this);
		}

		template <typename T>
		bool hasComponent() {
			return SceneManager::getActiveScene()->getECS().hasComponent<T>(*this);
		}

	private:
		UUID m_id;
	};
}