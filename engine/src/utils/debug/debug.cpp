#include "eepch.h"
#include "debug.h"
#include <algorithm>
#include <DbgHelp.h>
#include <iostream>
#pragma comment(lib, "Dbghelp.lib")

namespace emerald {
	static bool s_initialized = false;
	static std::unordered_map<size_t, std::string> s_stackTraceCache;

	void Debug::initialize() {
		if (!s_initialized) {
			HANDLE process = GetCurrentProcess();
			SymInitialize(process, NULL, TRUE);
			SymSetOptions(SYMOPT_LOAD_LINES | SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS);
			s_initialized = true;
		}
	}

	void Debug::shutdown() {
		if (s_initialized) {
			SymCleanup(GetCurrentProcess());
			s_stackTraceCache.clear();
			s_initialized = false;
		}
	}

	static size_t hashStackTrace(void** stack, unsigned short frameCount) {
		std::hash<void*> ptrHash;
		size_t hashValue = 0;

		for (unsigned short i = 0; i < frameCount; ++i) {
			hashValue ^= ptrHash(stack[i]) + 0x9e3779b9 + (hashValue << 6) + (hashValue >> 2);
		}

		return hashValue;
	}

	std::string Debug::captureStackTrace(uint32_t framesToSkip, uint32_t frameCount, bool writeAddress) {
		if (!s_initialized) return "Debug not initialized";

		// Capture the stack trace
		void* stack[64];
		unsigned short frames = CaptureStackBackTrace(framesToSkip, std::min((uint32_t)64, frameCount), stack, NULL);

		// Compute a hash for the current stack trace
		size_t stackHash = hashStackTrace(stack, frames);

		// Check if the stack trace is already cached
		auto it = s_stackTraceCache.find(stackHash);
		if (it != s_stackTraceCache.end()) {
			return it->second;
		}

		std::string result;
		result.reserve(512);  // Reserve space to avoid reallocations

		bool stopAtMain = false;

		// Prepare structures for line information
		IMAGEHLP_LINE64 line;
		DWORD displacementLine = 0;
		line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);

		for (unsigned short i = 0; i < frames && !stopAtMain; ++i) {
			DWORD64 address = (DWORD64)(stack[i]);

			// Symbol information
			char symbolBuffer[sizeof(SYMBOL_INFO) + 256 * sizeof(char)];
			SYMBOL_INFO* symbol = (SYMBOL_INFO*)symbolBuffer;
			symbol->MaxNameLen = 255;
			symbol->SizeOfStruct = sizeof(SYMBOL_INFO);

			DWORD64 displacement = 0;
			if (SymFromAddr(GetCurrentProcess(), address, &displacement, symbol)) {
				result += "at ";
				result += symbol->Name;

				if (writeAddress) {
					char addressBuffer[32];
					sprintf_s(addressBuffer, " - 0x%llx", symbol->Address);
					result += addressBuffer;
				}

				// Retrieve line number and file name
				if (SymGetLineFromAddr64(GetCurrentProcess(), address, &displacementLine, &line)) {
					result += " (";
					result += line.FileName;
					result += ":";
					result += std::to_string(line.LineNumber);
					result += ")";
				} else {
					result += " (no line info)";
				}

				result += "\n";

				if (std::string(symbol->Name).find("main") != std::string::npos) {
					stopAtMain = true;
				}
			} else {
				char addressBuffer[32];
				sprintf_s(addressBuffer, "at [0x%llx]\n", address);
				result += addressBuffer;
			}
		}

		s_stackTraceCache[stackHash] = result;

		return result;
	}
}