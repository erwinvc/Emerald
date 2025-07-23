#pragma once

namespace emerald {
	template <std::size_t N>
	class FixedString {
	public:
		using size_type = std::size_t;
		using value_type = char;
		using reference = char&;
		using const_reference = const char&;

		FixedString() noexcept {
			m_data[0] = '\0';
		}

		FixedString(const char* str) {
			assign(str);
		}

		FixedString(const FixedString& other) noexcept {
			assign(other.m_data);
		}

		FixedString& operator=(const FixedString& other) noexcept {
			if (this != &other) {
				assign(other.m_data);
			}
			return *this;
		}

		FixedString& operator=(const char* str) {
			assign(str);
			return *this;
		}

		constexpr size_type max_size() const noexcept {
			return N - 1;
		}

		size_type size() const noexcept {
			return std::strlen(m_data);
		}

		size_type length() const noexcept {
			return size();
		}

		constexpr size_type capacity() const noexcept {
			return N - 1;
		}

		bool empty() const noexcept {
			return m_data[0] == '\0';
		}

		// Element access
		reference operator[](size_type pos) {
			return m_data[pos];
		}

		const_reference operator[](size_type pos) const {
			return m_data[pos];
		}

		reference at(size_type pos) {
			if (pos >= size()) {
				throw std::out_of_range("FixedString::at: position out of range");
			}
			return m_data[pos];
		}

		const_reference at(size_type pos) const {
			if (pos >= size()) {
				throw std::out_of_range("FixedString::at: position out of range");
			}
			return m_data[pos];
		}

		reference front() {
			return m_data[0];
		}

		const_reference front() const {
			return m_data[0];
		}

		reference back() {
			return m_data[size() - 1];
		}

		const_reference back() const {
			return m_data[size() - 1];
		}

		const char* data() const noexcept {
			return m_data;
		}

		char* data() noexcept {
			return m_data;
		}

		const char* c_str() const noexcept {
			return m_data;
		}

		// Modifiers
		void clear() noexcept {
			m_data[0] = '\0';
		}

		void assign(const char* str) {
			if (str == nullptr) {
				m_data[0] = '\0';
				return;
			}

			size_type src_length = std::strlen(str);
			size_type copy_length = max_size() < src_length ? max_size() : src_length;

			std::memcpy(m_data, str, copy_length);
			m_data[copy_length] = '\0';
		}

		FixedString& operator+=(const char* str) {
			append(str);
			return *this;
		}

		FixedString& operator+=(char c) {
			push_back(c);
			return *this;
		}

		void push_back(char c) {
			size_type len = size();
			if (len < max_size()) {
				m_data[len] = c;
				m_data[len + 1] = '\0';
			}
		}

		void pop_back() {
			size_type len = size();
			if (len > 0) {
				m_data[len - 1] = '\0';
			}
		}

		FixedString& append(const char* str) {
			if (str == nullptr) {
				return *this;
			}

			size_type len = size();
			size_type src_length = std::strlen(str);
			size_type size_min_len = max_size() - len;
			size_type copy_length = size_min_len < src_length ? size_min_len : src_length;

			std::memcpy(m_data + len, str, copy_length);
			m_data[len + copy_length] = '\0'; // Ensure null-termination
			return *this;
		}

		FixedString& append(const FixedString& other) {
			return append(other.c_str());
		}

		// Operations
		FixedString substr(size_type pos = 0, size_type count = npos) const {
			if (pos > size()) {
				throw std::out_of_range("FixedString::substr: position out of range");
			}
			FixedString<N> result;
			size_type size_min_pos = size() - pos;
			size_type len = size_min_pos < count ? size_min_pos : count;
			std::memcpy(result.m_data, m_data + pos, len);
			result.m_data[len] = '\0';
			return result;
		}

		int compare(const FixedString& other) const noexcept {
			return std::strncmp(m_data, other.m_data, max_size());
		}

		// Find operations
		size_type find(char c, size_type pos = 0) const noexcept {
			for (size_type i = pos; i < size(); ++i) {
				if (m_data[i] == c) {
					return i;
				}
			}
			return npos;
		}

		size_type find(const char* s, size_type pos = 0) const noexcept {
			const char* found = std::strstr(m_data + pos, s);
			if (found) {
				return found - m_data;
			}
			return npos;
		}

		constexpr std::string_view view() const noexcept {
			return { m_data, size() };
		}
		operator std::string_view() const noexcept { return view(); }

		// Constants
		static constexpr size_type npos = static_cast<size_type>(-1);

		// Operator overloads
		operator const char* () const noexcept {
			return m_data;
		}

		bool operator==(const FixedString& other) const noexcept {
			return compare(other) == 0;
		}

		bool operator!=(const FixedString& other) const noexcept {
			return !(*this == other);
		}

		bool operator<(const FixedString& other) const noexcept {
			return compare(other) < 0;
		}

		bool operator>(const FixedString& other) const noexcept {
			return compare(other) > 0;
		}

		bool operator<=(const FixedString& other) const noexcept {
			return !(*this > other);
		}

		bool operator>=(const FixedString& other) const noexcept {
			return !(*this < other);
		}

	private:
		char m_data[N];
	};

}

template <std::size_t N>
inline emerald::FixedString<N> operator+(const emerald::FixedString<N>& lhs, const emerald::FixedString<N>& rhs) {
	emerald::FixedString<N> result(lhs);
	result += rhs.c_str();
	return result;
}

template <std::size_t N>
inline emerald::FixedString<N> operator+(const emerald::FixedString<N>& lhs, const char* rhs) {
	emerald::FixedString<N> result(lhs);
	result += rhs;
	return result;
}

template <std::size_t N>
inline emerald::FixedString<N> operator+(const char* lhs, const emerald::FixedString<N>& rhs) {
	emerald::FixedString<N> result(lhs);
	result += rhs.c_str();
	return result;
}

// Stream output operator
template <std::size_t N>
inline std::ostream& operator<<(std::ostream& os, const emerald::FixedString<N>& str) {
	return os << str.c_str();
}

namespace std {
	template <std::size_t N, class CharT>
	struct formatter<emerald::FixedString<N>, CharT>
		: formatter<std::basic_string_view<CharT>, CharT> {
		template <class FormatCtx>
		auto format(const emerald::FixedString<N>& str, FormatCtx& ctx) const {
			using SV = std::basic_string_view<CharT>;
			return formatter<SV, CharT>::format(SV(reinterpret_cast<const CharT*>(str.data()), str.size()), ctx);
		}
	};
} // namespace std