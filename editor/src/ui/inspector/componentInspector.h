#pragma once
#include "ui/imguiManager.h"
#include "inspector.h"
#include <algorithm>
#include "util/color.h"
#include "application.h"

namespace emerald {
	class Component;

	class ComponentInspector {
	};

	class DefaultComponentInspector : public ComponentInspector {
	public:
		static void draw(const std::vector<Component*>& components);
	};
};