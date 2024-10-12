#pragma once
#include "ref.h"

namespace emerald {
	class Asset : public RefCounted {
	private:

	public:
		UUID m_handle;
	};
}