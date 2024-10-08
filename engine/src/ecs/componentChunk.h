#pragma once
#include "flags.h"

namespace emerald {
	template<typename T, size_t ChunkSize = 1024>
	class ComponentChunk {
	public:
		size_t count;

		ComponentChunk() : count(0) {}

		bool isFull() const {
			return count >= ChunkSize;
		}

		T* getComponent(size_t index) {
			if (index >= count) return nullptr;
			T* components2 = reinterpret_cast<T*>(rawData);
			return &components2[index];
		}

		size_t getCount() const {
			return count;
		}

		T* getComponentsArray() {
			T* components = reinterpret_cast<T*>(rawData);
			return components;
		}

		void setEnabled(size_t index, bool enabled) {
			m_enabledFlags.setFlag(index, enabled);
		}

		void setEntityEnabled(size_t index, bool enabled) {
			m_entityEnabledFlags.setFlag(index, enabled);
		}

		bool isEnabled(size_t index) {
			return m_enabledFlags.isFlagSet(index) && m_entityEnabledFlags.isFlagSet(index);
		}

	private:
		alignas(T) char rawData[ChunkSize * sizeof(T)];
		Flags<ChunkSize> m_enabledFlags;
		Flags<ChunkSize> m_entityEnabledFlags;
	};
}