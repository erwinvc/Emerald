#pragma once

namespace emerald {
	template<size_t N>
	class Flags {
	public:
		Flags() { clearAllFlags(); }

		void setFlag(size_t index) {
			size_t idx = index / BitsPerWord;
			size_t bit = index % BitsPerWord;
			m_flags[idx] |= (WordType(1) << bit);
		}

		void setFlag(size_t index, bool value) {
			if (value)
				setFlag(index);
			else
				clearFlag(index);
		}

		void clearFlag(size_t index) {
			size_t idx = index / BitsPerWord;
			size_t bit = index % BitsPerWord;
			m_flags[idx] &= ~(WordType(1) << bit);
		}

		void clearAllFlags() {
			for (size_t i = 0; i < ArraySize; ++i)
				m_flags[i] = 0;
		}

		void toggleFlag(size_t index) {
			size_t idx = index / BitsPerWord;
			size_t bit = index % BitsPerWord;
			m_flags[idx] ^= (WordType(1) << bit);
		}

		bool isFlagSet(size_t index) const {
			size_t idx = index / BitsPerWord;
			size_t bit = index % BitsPerWord;
			return (m_flags[idx] & (WordType(1) << bit)) != 0;
		}

		bool isAnyFlagSet(const Flags<N>& flags) const {
			for (size_t i = 0; i < ArraySize; ++i) {
				if ((m_flags[i] & flags.m_flags[i]) != 0)
					return true;
			}
			return false;
		}

		bool areAllFlagsSet(const Flags<N>& flags) const {
			for (size_t i = 0; i < ArraySize; ++i) {
				if ((m_flags[i] & flags.m_flags[i]) != flags.m_flags[i])
					return false;
			}
			return true;
		}

		const uint32_t* getFlags() const { return m_flags; }

		Flags<N> operator|(const Flags<N>& other) const {
			Flags<N> result;
			for (size_t i = 0; i < ArraySize; ++i) {
				result.m_flags[i] = m_flags[i] | other.m_flags[i];
			}
			return result;
		}

		Flags<N> operator&(const Flags<N>& other) const {
			Flags<N> result;
			for (size_t i = 0; i < ArraySize; ++i) {
				result.m_flags[i] = m_flags[i] & other.m_flags[i];
			}
			return result;
		}

		Flags<N>& operator|=(const Flags<N>& other) {
			for (size_t i = 0; i < ArraySize; ++i) {
				m_flags[i] |= other.m_flags[i];
			}
			return *this;
		}

		Flags<N>& operator&=(const Flags<N>& other) {
			for (size_t i = 0; i < ArraySize; ++i) {
				m_flags[i] &= other.m_flags[i];
			}
			return *this;
		}

	private:
		using WordType = uint32_t;
		static constexpr size_t BitsPerWord = sizeof(WordType) * 8;
		static constexpr size_t ArraySize = (N + BitsPerWord - 1) / BitsPerWord;

		WordType m_flags[ArraySize];
	};

	using Flags8 = Flags<8>;
	using Flags16 = Flags<16>;
	using Flags32 = Flags<32>;
	using Flags64 = Flags<64>;
}