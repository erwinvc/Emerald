#pragma once
#include "rtti.h"
#include "ref.h"
#include "entity.h"

namespace emerald {
	class Component : public RefCounted {
		RTTI_BASE_CLASS_DECL(Component);
	public:
		Component() = default;
		Entity m_entity;
	};
}