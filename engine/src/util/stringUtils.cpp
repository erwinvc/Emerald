#include "eepch.h"
#include "stringUtils.h"

namespace emerald::stringUtils {
	std::string toLower(const std::string& str) {
		std::string result = str;
		std::transform(result.begin(), result.end(), result.begin(), ::tolower);
		return result;
	}
}