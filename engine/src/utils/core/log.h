#pragma once
#include <format>
#include <mutex>
#include <vector>
#include <chrono>

namespace emerald {
#define FOREGROUND_BLUE      0x0001 
#define FOREGROUND_GREEN     0x0002 
#define FOREGROUND_RED       0x0004 
#define FOREGROUND_INTENSITY 0x0008 
#define BACKGROUND_BLUE      0x0010 
#define BACKGROUND_GREEN     0x0020 
#define BACKGROUND_RED       0x0040 
#define BACKGROUND_INTENSITY 0x0080 
#undef ERROR

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

	enum class Severity {
		FATAL = 0,
		ERROR,
		WARN,
		INFO
	};

	struct LogMessage {
		std::string m_message = "";
		std::string m_stackTrace = "";
		std::string m_timestamp = "";
		Severity m_level = Severity::INFO;
	};

	/*Non-blocking logger*/
	class Log {
	public:
		struct QueuedMessage {
			ConsoleColor m_color;
			std::string m_message;
			std::string m_stackTrace;
			Severity m_level;
			std::chrono::system_clock::time_point m_time;

			QueuedMessage(ConsoleColor color, const std::string& message, const std::string& stackTrace, Severity level, std::chrono::system_clock::time_point time)
				: m_color(color), m_message(message), m_stackTrace(stackTrace), m_level(level), m_time(time) {
			}
			QueuedMessage()
				: m_color(ConsoleColor::WHITE), m_time(std::chrono::system_clock::now()), m_level(Severity::INFO) {
			}
		};

		static void initialize(const char* title);
		static void logMessage(Severity level, const std::string& message);
		static void forceEmptyQueue();
		static void shutdown();

		// TODO: This really needs a rewrite. Thread safety should not be handled by the caller.
		static std::vector<LogMessage>& getMessages();
		static void clearMessages();
		static std::mutex& getMessageMutex();

		template<typename... Args>
		static void info(std::format_string<Args...> format, Args&&... args) {
			logMessage(Severity::INFO, std::format(format, std::forward<Args>(args)...));
		}

		template<typename... Args>
		static void warn(std::format_string<Args...> format, Args&&... args) {
			logMessage(Severity::WARN, std::format(format, std::forward<Args>(args)...));
		}

		template<typename... Args>
		static void error(std::format_string<Args...> format, Args&&... args) {
			logMessage(Severity::ERROR, std::format(format, std::forward<Args>(args)...));
		}

		template<typename... Args>
		static void fatal(std::format_string<Args...> format, Args&&... args) {
			logMessage(Severity::FATAL, std::format(format, std::forward<Args>(args)...));
			Log::forceEmptyQueue();
			__debugbreak();
		}

	private:
		static void setTextColor(ConsoleColor color);
		static std::string getTimeAsString(std::chrono::system_clock::time_point currentTime);
		static void handleQueue();
		static void processMessage(const QueuedMessage& message);
	};
}

namespace std {
	template<>
	struct formatter<u8string> {
		constexpr auto parse(format_parse_context& ctx) {
			return ctx.begin();
		}

		auto format(const u8string& value, format_context& ctx) const {
			return std::format_to(ctx.out(), "{}", std::string(reinterpret_cast<const char*>(value.c_str())));
		}
	};
}