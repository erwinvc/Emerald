#pragma once
#include <string>

namespace emerald {
	class StringUtils {
	public:
		static inline std::string toLower(const std::string& str) {
			std::string result = str;
			std::transform(result.begin(), result.end(), result.begin(), ::tolower);
			return result;
		}
	};
}