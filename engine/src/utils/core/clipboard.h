#pragma once

namespace emerald {
	class Clipboard {
	public:
		static bool copyToClipboard(const std::string& text);
		static std::string getFromClipboard();
	};
}