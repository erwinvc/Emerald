#pragma once
#include "inspector.h"

namespace emerald {
	class Component;

	class ComponentInspector {
	};

	class DefaultComponentInspector : public ComponentInspector {
	public:
		static void draw(const std::vector<Component*>& components);
	};
};