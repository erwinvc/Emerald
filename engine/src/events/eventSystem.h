#pragma once
#include "events.h"
#define BIND_EVENT_HANDLER(fn) [this](auto& e) { this->fn(e); }

namespace emerald {
	class EventSystem {
	public:
		template<typename EventType>
		using EventCallback = std::function<void(EventType&)>;

		static bool dispatch(Event& e) {
			std::lock_guard<std::mutex> l(s_lock);

			// Dispatch to callback
			if (s_callback) {
				s_callback(e);
				if (e.isHandled()) return true;
			}

			// Dispatch to once subscribers
			auto itOnce = s_onceSubscribers.find(e.getClassType());
			if (itOnce != s_onceSubscribers.end()) {
				auto& callbacks = itOnce->second;
				auto callbackIter = callbacks.begin();
				while (callbackIter != callbacks.end()) {
					(*callbackIter)(e);
					callbackIter = callbacks.erase(callbackIter);  
					if (e.isHandled()) break;  
				}
			}

			// Dispatch to subscribers
			if (!e.isHandled()) {
				auto itRegular = s_subscribers.find(e.getClassType());
				if (itRegular != s_subscribers.end()) {
					auto& regularCallbacks = itRegular->second;
					for (auto& callback : regularCallbacks) {
						callback(e);
						if (e.isHandled()) break;
					}
				}
			}

			return e.isHandled();
		}

		template<typename EventType, typename InstanceType>
		static void routeEvent(Event& e, InstanceType* instance, void (InstanceType::* func)(EventType&)) {
			if (e.getClassType() == EventType::getStaticClassType()) {
				(instance->*func)(static_cast<EventType&>(e));
				return true;
			}
			return false;
		}

		template<typename EventType>
		static void routeEvent(Event& e, void (*func)(EventType&)) {
			if (e.getClassType() == EventType::getStaticClassType()) {
				func(static_cast<EventType&>(e));
				return true;
			}
			return false;
		}

		template<typename EventType, typename InstanceType>
		static bool handleEvent(Event& e, InstanceType* instance, void (InstanceType::* func)(EventType&)) {
			if constexpr (std::is_same_v<EventType, std::decay_t<decltype(e)>>) {
				(instance->*func)(static_cast<EventType&>(e));
				return true;
			}
			return false;
		}

		template<typename EventType>
		static void subscribe(const EventCallback<EventType>& callback) {
			std::lock_guard<std::mutex> l(s_lock);
			RTTIType type = EventType::getStaticClassType();
			s_subscribers[type].push_back([callback](Event& e) {
				callback(static_cast<EventType&>(e));
			});
		}

		template<typename EventType>
		static void subscribeOnce(const EventCallback<EventType>& callback) {
			std::lock_guard<std::mutex> l(s_lock);
			RTTIType type = EventType::getStaticClassType();
			s_onceSubscribers[type].push_back([callback](Event& e) {
				callback(static_cast<EventType&>(e));
			});
		}

		static void setGlobalCallback(const EventCallback<Event>& callback) { s_callback = callback; }

	private:
		static inline std::unordered_map<RTTIType, std::vector<std::function<void(Event&)>>> s_subscribers;
		static inline std::unordered_map<RTTIType, std::vector<std::function<void(Event&)>>> s_onceSubscribers;
		static inline EventCallback<Event> s_callback;
		static inline std::mutex s_lock;
	};
}