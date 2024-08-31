#pragma once

namespace emerald {
	class SceneManager;

	class Entity {
	public:
		Entity() : m_id(0) {}
		Entity(uint32_t id) : m_id(id) {}

		operator uint32_t() const { return m_id; }

		static Entity create(const std::string& name);

		//template <typename T>
		//void addComponent(const T& component) {
		//	SceneManager::getActiveScene()->getECS().addComponent<T>(*this, component);
		//}
		//
		//template <typename T>
		//void removeComponent() {
		//	SceneManager::getActiveScene()->getECS().removeComponent<T>(*this);
		//}
		//
		//template <typename T>
		//T& getComponent() {
		//	return SceneManager::getActiveScene()->getECS().getComponent<T>(*this);
		//}
		//
		//template <typename T>
		//bool hasComponent() {
		//	return SceneManager::getActiveScene()->getECS().hasComponent<T>(*this);
		//}

	private:
		uint32_t m_id;
	};
}