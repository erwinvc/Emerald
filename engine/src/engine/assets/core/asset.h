#pragma once
#include "utils/uuid/uuid.h"

namespace emerald {
	class Asset : public RefCounted {
	private:

	public:
		UUID m_handle;
	};
}