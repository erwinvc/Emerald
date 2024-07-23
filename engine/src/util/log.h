#pragma once
#include "datastructures/asyncQueue.h"
#include <consoleapi2.h>
#include <format>
#include <mutex>
#include <queue>
#include <fstream>
#include "threading/threadManager.h"

namespace emerald {
	class Thread;

	enum class ConsoleColor {
		BLACK = 0,
		DARKBLUE = FOREGROUND_BLUE,
		DARKGREEN = FOREGROUND_GREEN,
		DARKCYAN = FOREGROUND_GREEN | FOREGROUND_BLUE,
		DARKRED = FOREGROUND_RED,
		DARKMAGENTA = FOREGROUND_RED | FOREGROUND_BLUE,
		DARKYELLOW = FOREGROUND_RED | FOREGROUND_GREEN,
		DARKGRAY = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
		GRAY = FOREGROUND_INTENSITY,
		BLUE = FOREGROUND_INTENSITY | FOREGROUND_BLUE,
		GREEN = FOREGROUND_INTENSITY | FOREGROUND_GREEN,
		CYAN = FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE,
		RED = FOREGROUND_INTENSITY | FOREGROUND_RED,
		MAGENTA = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_BLUE,
		YELLOW = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN,
		WHITE = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
		RESET = 16
	};

	enum class LogLevel {
		FAIL = 0,
		WARN,
		INFO
	};

	/*Non-blocking logger with color formatting*/
	class Log {
	public:
		struct QueuedMessage {
			ConsoleColor m_color;
			std::string m_message;
			std::string m_type;
			std::chrono::system_clock::time_point m_time;
			QueuedMessage(ConsoleColor color, std::string message, std::string type, std::chrono::system_clock::time_point time) : m_color(color), m_message(message), m_type(type), m_time(time) {}
			QueuedMessage() : m_color(ConsoleColor::WHITE), m_time(std::chrono::system_clock::now()) {}
		};

		static void initialize(const char* title);
		static void logMessage(LogLevel level, const std::string& type, const std::string& message);
		static void forceEmptyQueue();
		static void cleanup();

		template<typename... Args>
		static void info(std::format_string<Args...> format, Args&&... args) {
			logMessage(LogLevel::INFO, "[Info]", std::format(format, std::forward<Args>(args)...));
		}

		template<typename... Args>
		static void warn(std::format_string<Args...> format, Args&&... args) {
			logMessage(LogLevel::WARN, "[Warn]", std::format(format, std::forward<Args>(args)...));
		}

		template<typename... Args>
		static void error(std::format_string<Args...> format, Args&&... args) {
			logMessage(LogLevel::FAIL, "[Fail]", std::format(format, std::forward<Args>(args)...));
			Log::forceEmptyQueue();
			__debugbreak();
		}

	private:
		static bool s_initialized;
		static bool s_shutdown;
		static LogLevel s_currentLogLevel;

		static std::string s_logPath;
		static std::ofstream m_logFile;

		static HANDLE s_outputHandle;
		static threading::Thread* s_outputThread;
		static CONSOLE_SCREEN_BUFFER_INFO s_screenBuffer;

		static AsyncQueue<Log::QueuedMessage> m_queue;

		static void setTextColor(ConsoleColor color);
		static std::string getTimeAsString(std::chrono::system_clock::time_point currentTime);
		static void handleQueue();
		static void processMessage(const QueuedMessage& message);
		static void setLogLevel(LogLevel level) {
			s_currentLogLevel = level;
		}
	};
}