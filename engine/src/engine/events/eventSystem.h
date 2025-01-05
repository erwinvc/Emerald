#pragma once
#include "events.h"
#include "utils/datastructures/asyncQueue.h"

namespace emerald {
	class EventSystem {
	public:
		template<typename EventType>
		using EventCallback = std::function<void(EventType&)>;

		template<typename EventType, typename... Args>
		static void dispatch(Args&&... args) {
			UniqueRef<EventType> e = UniqueRef<EventType>::create(std::forward<Args>(args)...);
			return dispatch(std::move(e));
		}

		static void dispatch(UniqueRef<Event>&& e) {
			s_eventQueue.emplace(std::move(e));
			//std::lock_guard<std::mutex> l(s_lock);
			//
			//// Dispatch to callback
			//if (s_callback) {
			//	s_callback(e);
			//	if (e.isHandled()) return true;
			//}
			//
			//// Dispatch to once subscribers
			//auto itOnce = s_onceSubscribers.find(e.getClassType());
			//if (itOnce != s_onceSubscribers.end()) {
			//	auto& callbacks = itOnce->second;
			//	auto callbackIter = callbacks.begin();
			//	while (callbackIter != callbacks.end()) {
			//		(*callbackIter)(e);
			//		callbackIter = callbacks.erase(callbackIter);
			//		if (e.isHandled()) break;
			//	}
			//}
			//
			//// Dispatch to subscribers
			//if (!e.isHandled()) {
			//	auto itRegular = s_subscribers.find(e.getClassType());
			//	if (itRegular != s_subscribers.end()) {
			//		auto& regularCallbacks = itRegular->second;
			//		for (auto& callback : regularCallbacks) {
			//			callback(e);
			//			if (e.isHandled()) break;
			//		}
			//	}
			//}
			//
			//return e.isHandled();
		}

		static void processEvents() {
			UniqueRef<Event> event;
			while (s_eventQueue.tryGet(event)) {
				dispatchEvent(*event);
			}
		}

		template<typename EventType, typename InstanceType>
		static bool routeEvent(Event& e, InstanceType* instance, void (InstanceType::* func)(EventType&)) {
			if (e.getClassType() == EventType::getStaticClassType()) {
				(instance->*func)(static_cast<EventType&>(e));
				return true;
			}
			return false;
		}

		template<typename EventType>
		static bool routeEvent(Event& e, void (*func)(EventType&)) {
			if (e.getClassType() == EventType::getStaticClassType()) {
				func(static_cast<EventType&>(e));
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

		template<typename EventType, typename InstanceType>
		static void subscribe(void (InstanceType::* func)(EventType&), InstanceType* instance) {
			std::lock_guard<std::mutex> l(s_lock);
			RTTIType type = EventType::getStaticClassType();
			s_subscribers[type].push_back([func, instance](Event& e) {
				(instance->*func)(static_cast<EventType&>(e));
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

		template<typename EventType, typename InstanceType>
		static void subscribeOnce(void (InstanceType::* func)(EventType&), InstanceType* instance) {
			std::lock_guard<std::mutex> l(s_lock);
			RTTIType type = EventType::getStaticClassType();
			s_onceSubscribers[type].push_back([func, instance](Event& e) {
				(instance->*func)(static_cast<EventType&>(e));
				});
		}

		static void setGlobalCallback(const EventCallback<Event>& callback) {
			std::lock_guard<std::mutex> l(s_lock);
			s_callback = callback;
		}

		template<typename InstanceType>
		static void setGlobalCallback(void (InstanceType::* func)(Event&), InstanceType* instance) {
			std::lock_guard<std::mutex> l(s_lock);
			s_callback = [instance, func](Event& e) { (instance->*func)(e); };
		}

	private:
		static inline std::unordered_map<RTTIType, std::vector<std::function<void(Event&)>>> s_subscribers;
		static inline std::unordered_map<RTTIType, std::vector<std::function<void(Event&)>>> s_onceSubscribers;
		static inline EventCallback<Event> s_callback;
		static inline std::mutex s_lock;
		static inline AsyncQueue<UniqueRef<Event>> s_eventQueue;

		static void dispatchEvent(Event& e) {
			std::lock_guard<std::mutex> l(s_lock);

			// Dispatch to global callback
			if (s_callback) {
				s_callback(e);
				if (e.isHandled()) return;
			}

			// Dispatch to once subscribers
			auto itOnce = s_onceSubscribers.find(e.getClassType());
			if (itOnce != s_onceSubscribers.end()) {
				auto& callbacks = itOnce->second;
				// Iterate callbacks, remove only the ones that handle the event,
				for (auto it = callbacks.begin(); it != callbacks.end(); ) {
					(*it)(e);
					if (e.isHandled()) {
						it = callbacks.erase(it);
						break;
					} else {
						++it;
					}
				}
			}

			// Dispatch to regular subscribers
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
		}
	};
}