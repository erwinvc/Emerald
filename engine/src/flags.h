#pragma once
#include "stdIncl.h"

namespace emerald {
	template<typename T>
	class Flags {
	public:
		Flags() : m_flags(0) {}
		Flags(T flags) : m_flags(flags) {}

		void setFlag(T flag) { m_flags |= flag; }
		void setFlag(T flag, bool value) { if (value) m_flags |= flag; else m_flags &= ~flag; }
		bool clearFlag(T flag) { return (m_flags &= ~flag) != 0; }
		void clearAllFlags() { m_flags = 0; }
		void toggleFlag(T flag) { m_flags ^= flag; }
		bool isFlagSet(T flag) const { return (m_flags & flag) != 0; }
		bool isAnyFlagSet(T flags) const { return (m_flags & flags) != 0; }
		bool areAllFlagsSet(T flags) const { return (m_flags & flags) == flags; }
		T getFlags() const { return m_flags; }

		operator T() const { return m_flags; }

	private:
		T m_flags;
	};

	typedef Flags<uint8_t> Flags8;
	typedef Flags<uint16_t> Flags16;
	typedef Flags<uint32_t> Flags32;
	typedef Flags<uint64_t> Flags64;
}