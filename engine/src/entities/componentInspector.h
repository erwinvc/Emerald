#pragma once

#define COMPONENT_INSPECTOR(ComponentClass, InspectorClass)						\
template<>																		\
struct ComponentInspectorType<ComponentClass> {using Type = InspectorClass;	};  \

namespace emerald {
	class Component;
	class ComponentInspector {
	public:
		virtual ~ComponentInspector() = default;
		virtual void renderInspector(Component* component) = 0;
	};

	class DefaultComponentInspector : public ComponentInspector {
	public:
		void renderInspector(Component* component) override {}
	};

	template<typename ComponentType>
	struct ComponentInspectorType {
		using Type = DefaultComponentInspector;
	};
};