#include "eepch.h"
#include "clipboard.h"

namespace emerald {
	bool Clipboard::copyToClipboard(const std::string& text) {
		if (!OpenClipboard(nullptr)) {
			return false;
		}

		EmptyClipboard();

		HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, text.size() + 1);
		if (!hGlobal) {
			CloseClipboard();
			return false;
		}

		memcpy(GlobalLock(hGlobal), text.c_str(), text.size() + 1);
		GlobalUnlock(hGlobal);

		SetClipboardData(CF_TEXT, hGlobal);

		CloseClipboard();
		return true;
	}

	std::string Clipboard::getFromClipboard() {
		if (!OpenClipboard(nullptr)) {
			return "";
		}

		HANDLE hData = GetClipboardData(CF_TEXT);
		if (!hData) {
			CloseClipboard();
			return "";
		}

		char* data = static_cast<char*>(GlobalLock(hData));
		if (!data) {
			CloseClipboard();
			return "";
		}

		std::string clipboardText(data);
		GlobalUnlock(hData);

		CloseClipboard();
		return clipboardText;
	}
}