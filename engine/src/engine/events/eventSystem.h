#pragma once
#include "events.h"
#include "utils/datastructures/asyncQueue.h"
#include "utils/datastructures/vector.h"

namespace emerald {
	class EventSystem {
	public:
		template<typename EventType>
		using EventCallback = std::function<void(EventType&)>;

		using SubscriptionID = uint64_t;

		template<typename EventType, typename... Args>
		static void dispatch(Args&&... args) {
			UniqueRef<EventType> e = UniqueRef<EventType>::create(std::forward<Args>(args)...);
			return dispatch(std::move(e));
		}

		static void dispatch(UniqueRef<Event>&& e) {
			s_eventQueue.emplace(std::move(e));
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
		static SubscriptionID subscribe(const EventCallback<EventType>& callback) {
			std::lock_guard<std::mutex> l(s_lock);
			RTTIType type = EventType::getStaticClassType();
			SubscriptionID id = generateSubscriptionID();
			s_subscribers[type].pushBack({ id, [callback](Event& e) {
				callback(static_cast<EventType&>(e));
			} });
			return id;
		}

		template<typename EventType, typename InstanceType>
		static SubscriptionID subscribe(void (InstanceType::* func)(EventType&), InstanceType* instance) {
			std::lock_guard<std::mutex> l(s_lock);
			RTTIType type = EventType::getStaticClassType();
			SubscriptionID id = generateSubscriptionID();
			s_subscribers[type].pushBack({ id, [func, instance](Event& e) {
				(instance->*func)(static_cast<EventType&>(e));
			} });
			return id;
		}

		template<typename EventType>
		static SubscriptionID subscribeOnce(const EventCallback<EventType>& callback) {
			std::lock_guard<std::mutex> l(s_lock);
			RTTIType type = EventType::getStaticClassType();
			SubscriptionID id = generateSubscriptionID();
			s_onceSubscribers[type].pushBack({ id, [callback](Event& e) {
				callback(static_cast<EventType&>(e));
			} });
			return id;
		}

		template<typename EventType, typename InstanceType>
		static SubscriptionID subscribeOnce(void (InstanceType::* func)(EventType&), InstanceType* instance) {
			std::lock_guard<std::mutex> l(s_lock);
			RTTIType type = EventType::getStaticClassType();
			SubscriptionID id = generateSubscriptionID();
			s_onceSubscribers[type].pushBack({ id, [func, instance](Event& e) {
				(instance->*func)(static_cast<EventType&>(e));
			} });
			return id;
		}

		static void unsubscribe(SubscriptionID id) {
			std::lock_guard<std::mutex> l(s_lock);

			// Search in regular subscribers
			for (auto& [type, subscribers] : s_subscribers) {
				subscribers.removeIf([id](const auto& sub) { return sub.id == id; });
			}

			// Search in once subscribers
			for (auto& [type, subscribers] : s_onceSubscribers) {
				subscribers.removeIf([id](const auto& sub) { return sub.id == id; });
			}
		}

		template<typename EventType>
		static void unsubscribeAll() {
			std::lock_guard<std::mutex> l(s_lock);
			RTTIType type = EventType::getStaticClassType();
			s_subscribers.erase(type);
			s_onceSubscribers.erase(type);
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
		struct Subscription {
			SubscriptionID id;
			std::function<void(Event&)> callback;
		};

		static inline std::atomic<SubscriptionID> s_nextSubscriptionID{ 1 };
		static inline std::unordered_map<RTTIType, Vector<Subscription>> s_subscribers;
		static inline std::unordered_map<RTTIType, Vector<Subscription>> s_onceSubscribers;
		static inline EventCallback<Event> s_callback;
		static inline std::mutex s_lock;
		static inline AsyncQueue<UniqueRef<Event>> s_eventQueue;

		static SubscriptionID generateSubscriptionID() {
			return s_nextSubscriptionID++;
		}

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
				auto& subscribers = itOnce->second;
				for (auto it = subscribers.begin(); it != subscribers.end(); ) {
					it->callback(e);
					if (e.isHandled()) {
						it = subscribers.erase(it);
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
					auto& subscribers = itRegular->second;
					for (const auto& sub : subscribers) { 
						sub.callback(e);  
						if (e.isHandled()) break;
					}
				}
			}
		}
	};
}