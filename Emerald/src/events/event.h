
enum class EventType {
    None = 0,
    WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
    AppTick, AppUpdate, AppRender,
    KeyPressed, KeyReleased,
    MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
};

enum EventCategory {
    None = 0,
    EventCategoryApplication = BIT(0),
    EventCategoryInput = BIT(1),
    EventCategoryKeyboard = BIT(2),
    EventCategoryMouse = BIT(3),
    EventCategoryMouseButton = BIT(4)
};

#define EVENT_CLASS_TYPE(type) static EventType GetStaticType() { return EventType::##type; }\
								virtual EventType GetEventType() override { return GetStaticType(); }\
								virtual String GetName() override { return #type; }

#define EVENT_CLASS_CATEGORY(category) virtual int GetCategoryFlags() override { return category; }

class Event {
    friend class EventDispatcher;
public:
    virtual EventType GetEventType() = 0;
    virtual String GetName() = 0;
    virtual int GetCategoryFlags() = 0;

    inline bool IsInCategory(EventCategory category) {
        return GetCategoryFlags() & category;
    }

    template<typename T>
    void Dispatch(std::function<bool(T&)> func) {
        if(m_handled) return;
        if (GetEventType() == T::GetStaticType()) {
            m_handled = func(*(T*)this);
        }
    }
protected:
    bool m_handled = false;
};