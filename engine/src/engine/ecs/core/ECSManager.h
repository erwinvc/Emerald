#pragma once
#include "core/common/staticClass.h"

namespace emerald {
	class EntityComponentSystem;

	class ECSManager {
		STATIC_CLASS(ECSManager)
	public:
		static void Initialize(bool isEditor);
		static void Shutdown();
		static void SetActiveECS(bool playMode);

		static EntityComponentSystem& ECS();
		static EntityComponentSystem& EditorECS();
		static EntityComponentSystem& PlayECS();
	};
}