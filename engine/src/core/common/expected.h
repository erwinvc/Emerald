#pragma once
#include <variant>
#include <string>
#include <stdexcept>
#include <format>
#include <utility>

namespace emerald {
	using Empty = std::monostate;

	class Unexpected {
	public:
		explicit Unexpected(std::string err) : m_error(std::move(err)) {}

		explicit Unexpected(const char* err) : m_error(err) {}

		template <typename... Args>
		Unexpected(std::format_string<Args...> fmt, Args&&... args)
			: m_error(std::format(fmt, std::forward<Args>(args)...)) {
		}

		const std::string& value() const noexcept {
			return m_error;
		}

	private:
		std::string m_error;
	};

	template <typename T>
	class Expected {
	public:
		Expected() : m_variant(std::in_place_index<0>, T()) {}
		Expected(const T& val)
			: m_variant(std::in_place_index<0>, val) {
		}

		Expected(T&& val)
			: m_variant(std::in_place_index<0>, std::move(val)) {
		}

		Expected(const Unexpected& unexp)
			: m_variant(std::in_place_index<1>, unexp.value()) {
		}

		Expected(Unexpected&& unexp)
			: m_variant(std::in_place_index<1>, std::move(unexp.value())) {
		}


		bool hasValue() const noexcept {
			return m_variant.index() == 0;
		}

		explicit operator bool() const noexcept {
			return hasValue();
		}

		//================================================================
		  // value() Accessors -- Reference-Qualified
		  //================================================================
		T& value()& {
			if (!hasValue()) {
				throw std::logic_error("bad expected access: no value");
			}
			return std::get<0>(m_variant);
		}

		const T& value() const& {
			if (!hasValue()) {
				throw std::logic_error("bad expected access: no value");
			}
			return std::get<0>(m_variant);
		}

		T&& value()&& {
			if (!hasValue()) {
				throw std::logic_error("bad expected access: no value");
			}
			return std::move(std::get<0>(m_variant));
		}

		const T&& value() const&& {
			if (!hasValue()) {
				throw std::logic_error("bad expected access: no value");
			}
			return std::move(std::get<0>(m_variant));
		}

		//================================================================
		// error() Accessors -- (Optional) Reference-Qualified
		//================================================================
		std::string& error()& {
			if (hasValue()) {
				throw std::logic_error("bad expected access: has value");
			}
			return std::get<1>(m_variant);
		}

		const std::string& error() const& {
			if (hasValue()) {
				throw std::logic_error("bad expected access: has value");
			}
			return std::get<1>(m_variant);
		}

		std::string&& error()&& {
			if (hasValue()) {
				throw std::logic_error("bad expected access: has value");
			}
			return std::move(std::get<1>(m_variant));
		}

		const std::string&& error() const&& {
			if (hasValue()) {
				throw std::logic_error("bad expected access: has value");
			}
			return std::move(std::get<1>(m_variant));
		}

		//================================================================
		// valueOr(...) Methods
		//================================================================

		T valueOr(const T& fallback) const& {
			if (hasValue()) {
				return std::get<0>(m_variant);
			}
			return fallback;
		}

		T valueOr(T&& fallback)&& {
			if (hasValue()) {
				return std::move(std::get<0>(m_variant));
			}
			return std::move(fallback);
		}


	private:
		std::variant<T, std::string> m_variant;
	};
}