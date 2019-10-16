//#pragma once
#define LOG( fmt, ...)					Logger::Message((int)ConsoleColor::WHITE,				" [Info]",		fmt, ##__VA_ARGS__ )
#define LOG_WARN( fmt, ...)				Logger::Message((int)ConsoleColor::RED,					" [Warn]",		fmt, ##__VA_ARGS__ )
#define LOG_TIMED(time, fmt, ...)		Logger::MessageTimed(time, (int)ConsoleColor::WHITE,	" [Info]",		fmt, ##__VA_ARGS__ )
#define LOG_ERROR( fmt, ...)			{Logger::Message((int)ConsoleColor::RED,			" [Fail]",		fmt, ##__VA_ARGS__ ); Logger::ForceEmptyQueue(); __debugbreak();}

class UILoggerComponent;

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


class Logger {
public:
	struct QueuedMessage {
		int m_color;
		String m_message;
		String m_type;
		time_t m_time;
		QueuedMessage(int color, String message, String type, time_t time) : m_color(color), m_message(message), m_type(type), m_time(time) {}
		QueuedMessage() : m_color(0), m_message(""), m_type(""), m_time(0) {}
	};
	static const int MAXQUEUESIZE = 1000;
	static void Initialize();
	static void Message(int color, const char* type, const char* fmt, ...);
	static void MessageTimed(int time, int color, const char* type, const char* fmt, ...);
	static void ForceEmptyQueue();
	static void Cleanup();
	static void OnImGui();


	template <typename... Args>
	static void LogMessage(Args... args) {
		char buffer[1024 * 10];
		int32 position = 0;
		print_log_internal(buffer, position, forward<Args>(args)...);

		//buffer[position++] = '\n';
		buffer[position] = 0;

		AddToQueue(ConsoleColor::WHITE, buffer, "[Info]", time(nullptr));
	}

private:
	static FILE *m_stream;
	static HANDLE m_outputHandle;
	static HANDLE m_inputHandle;
	static Thread* m_inputThread;
	static Thread* m_outputThread;
	static CONSOLE_SCREEN_BUFFER_INFO m_screenBuffer;
	static bool m_allocated;
	static bool m_firstEntry;
	static bool m_stopping;

	static void LogToFile(const char * buff);
	static void SetTextColor(const int color);
	static const char* GetTimeAsString(time_t& currentTime);

	//Queue
	static mutex m_mutex;
	static condition_variable m_conditionVariable;
	static queue<QueuedMessage> m_queue;
	static void HandleQueue();
	static void AddToQueue(int color, const String& message, const String& type, time_t time);
	static void ProcessMessage(QueuedMessage& message);

	template <typename First>
	static void print_log_internal(char* buffer, int32& position, First&& first) {
		const char* formatted = ToString<First>(first);
		int32 length = strlen(formatted);
		memcpy(&buffer[position], formatted, length);
		position += length;
	}

	template <typename First, typename... Args>
	static void print_log_internal(char* buffer, int32& position, First&& first, Args&&... args) {
		const char* formatted = ToString<First>(first);
		int length = strlen(formatted);
		memcpy(&buffer[position], formatted, length);
		position += length;
		if (sizeof...(Args))
			print_log_internal(buffer, position, forward<Args>(args)...);
	}

	template <typename T>
	static const char* ToString(const T& t) {
		return Format_t("~r [%d] ~x?", typeid(T).name());
	}

	//Primitives
	template <>
	static String_t ToString<bool>(const bool& t) {
		return t ? "true" : "false";
	}

	template <>
	static String_t ToString<int>(const int& t) {
		return Format_t("%d", t);
	}

	template <>
	static String_t ToString<long>(const long& t) {
		return Format_t("%ld", t);
	}

	template <>
	static String_t ToString<float>(const float& t) {
		return Format_t("%f", t);
	}

	template <>
	static String_t ToString<double>(const double& t) {
		return Format_t("%lf", t);
	}

	//Text
	template <>
	static String_t ToString<char>(const char& t) {
		return Format_t("%c", t);
	}

	template <>
	static String_t ToString<char*>(char* const& t) {
		return t;
	}

	template <>
	static String_t ToString<String_t>(String_t const& t) {
		return t;
	}

	template <>
	static String_t ToString<String>(String const& t) {
		return t.c_str();
	}

	//Game types
	template <>
	static String_t ToString<uint>(uint const& t) {
		return Format_t("%u", t);
	}

	template <>
	static String_t ToString<Vector2>(Vector2 const& t) {
		return Format_t("(%f, %f)", t.x, t.y);
	}

	template <>
	static String_t ToString<Vector2I>(Vector2I const& t) {
		return Format_t("(%d, %d)", t.x, t.y);
	}

	template <>
	static String_t ToString<Vector3>(Vector3 const& t) {
		return Format_t("(%f, %f, %f)", t.x, t.y, t.z);
	}

	template <>
	static String_t ToString<Vector4>(Vector4 const& t) {
		return Format_t("(%f, %f, %f, %f)", t.x, t.y, t.z, t.w);
	}

	template <>
	static String_t ToString<Rect>(Rect const& t) {
		return Format_t("(%f, %f, %f, %f)", t.m_position.x, t.m_position.y, t.m_size.x, t.m_size.y);
	}

	template <>
	static String_t ToString<ConsoleColor>(ConsoleColor const& t) {
		switch (t) {
		case ConsoleColor::BLACK: return "~3";
		case ConsoleColor::DARKBLUE: return "~B";
		case ConsoleColor::DARKGREEN: return "~G";
		case ConsoleColor::DARKCYAN: return "~C";
		case ConsoleColor::DARKRED: return "~R";
		case ConsoleColor::DARKMAGENTA: return "~M";
		case ConsoleColor::DARKYELLOW: return "~Y";
		case ConsoleColor::DARKGRAY: return "~2";
		case ConsoleColor::GRAY: return "~1";
		case ConsoleColor::BLUE: return "~b";
		case ConsoleColor::GREEN: return "~g";
		case ConsoleColor::CYAN: return "~c";
		case ConsoleColor::RED: return "~r";
		case ConsoleColor::MAGENTA: return "~m";
		case ConsoleColor::YELLOW: return "~y";
		case ConsoleColor::WHITE: return "~0";
		case ConsoleColor::RESET: return "~x";
		}
	}
};
