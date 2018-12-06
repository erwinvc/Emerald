//#pragma once
#define LOG( fmt, ...)					Logger::Message(ConsoleColor::WHITE,		" [Info]",		fmt, ##__VA_ARGS__ )
#define LOG_PRINT( fmt, ...)			Logger::Message(ConsoleColor::WHITE,		" [Info]",		fmt, ##__VA_ARGS__ )
#define LOG_DEBUG( fmt, ...)			Logger::Message(ConsoleColor::GRAY,		" [Debg]",		fmt, ##__VA_ARGS__ )
#define LOG_WARN( fmt, ...)				Logger::Message(ConsoleColor::DARKYELLOW,	" [Warn]",		fmt, ##__VA_ARGS__ )
#define LOG_ERROR( fmt, ...)			Logger::Message(ConsoleColor::RED,		" [Fail]",		fmt, ##__VA_ARGS__ )
#define LOG_CUSTOM( color, fmt, ...)	Logger::Message(color,								" [Info]",		fmt, ##__VA_ARGS__ )

class UILoggerComponent;

namespace ConsoleColor {
	enum {
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
	};
}

class Logger {
public:
	static void Initialize();
	static void Message(int color, const char* type, const char* fmt, ...);
	static void LogToFile(const char * buff);
	static void Cleanup();

private:
	static FILE *m_stream;
	static HANDLE m_outputHandle;
	static HANDLE m_inputHandle;
	static bool m_allocated;
	static bool m_firstEntry;

	static void SetTextColor(const int color);
	static const char* GetTimeAsString();
};
