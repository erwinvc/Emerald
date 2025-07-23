#pragma once

namespace emerald {
	template<size_t N>
	class Flags {
	public:
		Flags() { clearAllFlags(); }
		Flags(size_t flags) : m_flags(flags) {}

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

		template <class E>
		void setFlag(E e) { setFlag(toIndex(e)); }

		template <class E>
		void setFlag(E e, bool v) { setFlag(toIndex(e), v); }

		template<class... E>
		void setFlags(E... es) { (setFlag(toIndex(es)), ...); }

		template <class E>
		void clearFlag(E e) { clearFlag(toIndex(e)); }

		template <class E>
		void toggleFlag(E e) { toggleFlag(toIndex(e)); }

		template <class E>
		bool isFlagSet(E e) const { return isFlagSet(toIndex(e)); }

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
		template<class E>
		static constexpr std::size_t toIndex(E e) noexcept {
			static_assert(std::is_enum_v<E>);
			using UT = std::underlying_type_t<E>;
			UT value = static_cast<UT>(e);

			// require exactly one bit set
			assert(value && (value & (value - 1)) == 0 && "Enum is not a power-of-two mask");

			return std::countr_zero(static_cast<std::make_unsigned_t<UT>>(value));
		}

		using WordType = uint32_t;
		static constexpr size_t BitsPerWord = sizeof(WordType) * 8;
		static constexpr size_t ArraySize = (N + BitsPerWord - 1) / BitsPerWord;

		WordType m_flags[ArraySize];
	};

	using Flags8 = Flags<8>;
	using Flags16 = Flags<16>;
	using Flags32 = Flags<32>;
	using Flags64 = Flags<64>;

	template<size_t N>
	void to_json(nlohmann::json& j, const Flags<N>& f) {
		std::string bitstring(N, '0');
		for (int i = N - 1; i >= 0; --i) {
			bitstring[N - 1 - i] = f.isFlagSet(size_t(i)) ? '1' : '0';
		}
		j = bitstring;
	}

	template<size_t N>
	void from_json(const nlohmann::json& j, Flags<N>& f) {
		if (!j.is_string()) {
			throw std::runtime_error(std::format("Expected a string for Flags<{}>", N));
		}
		std::string bitstring = j.get<std::string>();
		if (bitstring.size() != N) {
			throw std::runtime_error(std::format("Flags<{}> string must be length {}", N, N));
		}
		f.clearAllFlags();
		for (int i = 0; i < N; i++) {
			if (bitstring[i] == '1') {
				f.setFlag(N - 1 - i);
			}
		}
	}
}