#pragma once

struct GLFWwindow;
namespace emerald {
	class Window;

	class GLWindowCallbacks {
	private:

		std::vector<std::function<void(uint32_t, uint32_t)>>						m_onResizeCallbacks;
		std::vector<std::function<void()>>											m_onCloseCallbacks;
		std::vector<std::function<void(uint32_t, uint32_t, uint32_t, uint32_t)>>	m_onKeyCallbacks;
		std::vector<std::function<void(uint32_t, uint32_t, uint32_t)>>				m_onMouseButtonCallbacks;
		std::vector<std::function<void(double, double)>>							m_onMousePosCallbacks;
		std::vector<std::function<void(double, double)>>							m_onScrollCallbacks;
		std::vector<std::function<void(uint32_t)>>									m_onKeyTypedCallbacks;
		std::vector<std::function<void(uint32_t)>>									m_onFocusCallbacks;

		static const GLWindowCallbacks& getWindowCallbacks(GLFWwindow* wnd);

	public:
		void initialize(Window* window);

		template<class T> void addOnResizeCallback(T* object, void(T::* callback)(uint32_t, uint32_t)) {
			m_onResizeCallbacks.emplace_back(std::bind(callback, object, std::placeholders::_1, std::placeholders::_2));
		}
		template<class T> void addOnCloseCallback(T* object, void(T::* callback)()) {
			m_onCloseCallbacks.emplace_back(std::bind(callback, object));
		}
		template<class T> void addOnKeyCallback(T* object, void(T::* callback)(uint32_t, uint32_t, uint32_t, uint32_t)) {
			m_onKeyCallbacks.emplace_back(std::bind(callback, object, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
		}
		template<class T> void addOnMouseButtonCallback(T* object, void(T::* callback)(uint32_t, uint32_t, uint32_t)) {
			m_onMouseButtonCallbacks.emplace_back(std::bind(callback, object, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
		}
		template<class T> void addOnMousePosCallback(T* object, void(T::* callback)(double, double)) {
			m_onMousePosCallbacks.emplace_back(std::bind(callback, object, std::placeholders::_1, std::placeholders::_2));
		}
		template<class T> void addOnScrollCallback(T* object, void(T::* callback)(double, double)) {
			m_onScrollCallbacks.emplace_back(std::bind(callback, object, std::placeholders::_1, std::placeholders::_2));
		}
		template<class T> void addOnKeyTypedEvent(T* object, void(T::* callback)(uint32_t)) {
			m_onKeyTypedCallbacks.emplace_back(std::bind(callback, object, std::placeholders::_1));
		}
		template<class T> void addOnFocusCallback(T* object, void(T::* callback)(uint32_t)) {
			m_onFocusCallbacks.emplace_back(std::bind(callback, object, std::placeholders::_1));
		}

		void addOnResizeCallback(std::function<void(uint32_t, uint32_t)> callback) { m_onResizeCallbacks.push_back(std::move(callback)); }
		void addOnCloseCallback(std::function<void()> callback) { m_onCloseCallbacks.push_back(std::move(callback)); }
		void addOnKeyCallback(std::function<void(uint32_t, uint32_t, uint32_t, uint32_t)> callback) { m_onKeyCallbacks.push_back(std::move(callback)); }
		void addOnMouseButtonCallback(std::function<void(uint32_t, uint32_t, uint32_t)> callback) { m_onMouseButtonCallbacks.push_back(std::move(callback)); }
		void addOnMousePosCallback(std::function<void(double, double)> callback) { m_onMousePosCallbacks.push_back(std::move(callback)); }
		void addOnScrollCallback(std::function<void(double, double)> callback) { m_onScrollCallbacks.push_back(std::move(callback)); }
		void addOnKeyTypedEvent(std::function<void(uint32_t)> callback) { m_onKeyTypedCallbacks.push_back(std::move(callback)); }
		void addOnFocusCallback(std::function<void(uint32_t)> callback) { m_onFocusCallbacks.push_back(std::move(callback)); }
	};
}