#pragma once
#include "rtti.h"
#include "ref.h"
#include "ecs/entity.h"

namespace emerald {
	class Component : public RefCounted {
		RTTI_BASE_CLASS_DECL(Component);
	public:
		Component() = default;
		bool m_enabled;
		Entity m_entity;
	};
}