#pragma once

class GLCallbackManager : public Singleton<GLCallbackManager> {
private:
	GLCallbackManager() {}
	~GLCallbackManager() {}
	friend Singleton;

    vector<function<void(int, int)>>            m_onResizeCallbacks;
    vector<function<void()>>                    m_onCloseCallbacks;
    vector<function<void(int, int, int, int)>>  m_onKeyCallbacks;
    vector<function<void(int, int, int)>>       m_onMouseButtonCallbacks;
    vector<function<void(double, double)>>      m_onMousePosCallbacks;
    vector<function<void(double, double)>>      m_onScrollCallbacks;
	vector<function<void(uint)>>                m_onKeyTypedCallbacks;
	vector<function<void(int)>>					m_onFocusCallbacks;

public:
    void Initialize(Window* window);

    template<class T> void AddOnResizeCallback(T* object, void(T::* callback)(int, int)) {
        m_onResizeCallbacks.emplace_back(std::bind(callback, object, placeholders::_1, placeholders::_2));
    }
    template<class T> void AddOnCloseCallback(T* object, void(T::* callback)()) {
        m_onCloseCallbacks.emplace_back(std::bind(callback, object));
    }
    template<class T> void AddOnKeyCallback(T* object, void(T::*callback)(int, int, int, int)) {
        m_onKeyCallbacks.emplace_back(std::bind(callback, object, placeholders::_1, placeholders::_2, placeholders::_3, placeholders::_4));
    }
    template<class T> void AddOnMouseButtonCallback(T* object, void(T::* callback)(int, int, int)) {
        m_onMouseButtonCallbacks.emplace_back(std::bind(callback, object, placeholders::_1, placeholders::_2, placeholders::_3));
    }
    template<class T> void AddOnMousePosCallback(T* object, void(T::* callback)(double, double)) {
        m_onMousePosCallbacks.emplace_back(std::bind(callback, object, placeholders::_1, placeholders::_2));
    }
    template<class T> void AddOnScrollCallback(T* object, void(T::* callback)(double, double)) {
        m_onScrollCallbacks.emplace_back(std::bind(callback, object, placeholders::_1, placeholders::_2));
    }
    template<class T> void AddOnKeyTypedEvent(T* object, void(T::* callback)(uint)) {
        m_onKeyTypedCallbacks.emplace_back(std::bind(callback, object, placeholders::_1));
    }

	template<class T> void AddOnFocusCallback(T* object, void(T::* callback)(int)) {
		m_onFocusCallbacks.emplace_back(std::bind(callback, object, placeholders::_1));
	}

    void AddOnResizeCallback(function<void(int, int)> callback)             { m_onResizeCallbacks.push_back(move(callback)); }
    void AddOnCloseCallback(function<void()> callback)                      { m_onCloseCallbacks.push_back(move(callback)); }
    void AddOnKeyCallback(function<void(int, int, int, int)> callback)      { m_onKeyCallbacks.push_back(move(callback)); }
    void AddOnMouseButtonCallback(function<void(int, int, int)> callback)   { m_onMouseButtonCallbacks.push_back(move(callback)); }
    void AddOnMousePosCallback(function<void(double, double)> callback)     { m_onMousePosCallbacks.push_back(move(callback)); }
    void AddOnScrollCallback(function<void(double, double)> callback)       { m_onScrollCallbacks.push_back(move(callback)); }
	void AddOnKeyTypedEvent(function<void(uint)> callback)					{ m_onKeyTypedCallbacks.push_back(move(callback)); }
	void AddOnFocusCallback(function<void(int)> callback)						{ m_onFocusCallbacks.push_back(move(callback)); }
};

static GLCallbackManager* GetGLCallbackManager() {
    return GLCallbackManager::GetInstance();
}
