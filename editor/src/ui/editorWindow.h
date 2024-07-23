#pragma once
#include "emerald.h"

namespace emerald {
	class EditorWindow {
	public:
		EditorWindow() = default;
		~EditorWindow() = default;

		void initialize();
		void update(Timestep ts);
		void fixedUpdate(Timestep ts);
		void destroy();
	};
}