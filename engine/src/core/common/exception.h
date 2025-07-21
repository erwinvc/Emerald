#pragma once
#include <stdexcept>
#include <format>

namespace emerald {
	class EmeraldError : public std::runtime_error {
	public:
		template <typename... Args>
		EmeraldError(std::format_string<Args...> fmt, Args&&... args)
			: std::runtime_error(std::format(fmt, std::forward<Args>(args)...)) {
		}
		EmeraldError(const char* message)
			: std::runtime_error(message) {
		}
		EmeraldError(const std::string& message)
			: std::runtime_error(message) {
		}
	};
}