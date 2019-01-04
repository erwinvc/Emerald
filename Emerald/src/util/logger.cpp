#include "stdafx.h"

bool Logger::m_allocated = false;
bool Logger::m_firstEntry = true;

HANDLE Logger::m_outputHandle = INVALID_HANDLE_VALUE;
HANDLE Logger::m_inputHandle = INVALID_HANDLE_VALUE;

DWORD WINAPI HandleConsoleInput(LPVOID hModule) {
	String input;
	while (true) {
		getline(cin, input);
		LOG_PRINT("~R%s", input.c_str());
	}
	return 0;
}

void Logger::Initialize() {
	if (m_allocated) return;

	AllocConsole();

	SetConsoleTitleA("Emerald");

	m_outputHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	m_inputHandle = GetStdHandle(STD_INPUT_HANDLE);

	const int width = 120;
	const int height = 30;

	m_allocated = true;
	m_firstEntry = true;

	LOG_PRINT("[~gLogging~x] Console allocated");

	CreateThread(0, 0, HandleConsoleInput, 0, 0, 0);
}



void Logger::SetTextColor(const int color) {
	if (!m_allocated) return;

	CONSOLE_SCREEN_BUFFER_INFO screenBuffer;
	GetConsoleScreenBufferInfo(m_outputHandle, &screenBuffer);

	WORD attributes = screenBuffer.wAttributes & ~FOREGROUND_RED & ~FOREGROUND_GREEN & ~FOREGROUND_BLUE & ~FOREGROUND_INTENSITY;
	attributes |= color;

	SetConsoleTextAttribute(m_outputHandle, attributes);
}

const char* Logger::GetTimeAsString() {
	struct tm timeStruct;
	time_t currentTime = time(nullptr);
	localtime_s(&timeStruct, &currentTime);

	static char buffer[48];
	sprintf_s(buffer, "[%02d:%02d:%02d]", timeStruct.tm_hour, timeStruct.tm_min, timeStruct.tm_sec);
	return buffer;
}

static vector<String> split(String& s, String splitter) {
	size_t pos;
	vector<String> out;
	while ((pos = s.find(splitter)) != String::npos) {
		String token = s.substr(0, pos);
		out.push_back(token);
		s.erase(0, pos + splitter.length());
	}
	out.push_back(s);
	return out;
}


std::string ReplaceString(std::string subject, const std::string& search,
	const std::string& replace) {
	size_t pos = 0;
	while ((pos = subject.find(search, pos)) != std::string::npos) {
		subject.replace(pos, search.length(), replace);
		pos += replace.length();
	}
	return subject;
}

void Logger::Message(int color, const char* type, const char* fmt, ...) {
	char buffer[512] = { 0 };
	va_list va_alist;

	va_start(va_alist, fmt);
	vsprintf_s(buffer, fmt, va_alist);
	va_end(va_alist);
	String_t time = GetTimeAsString();
	String final;
	String str(buffer);
	String newStr = ReplaceString(str, "~", "`~");
	vector<String> spl = split(newStr, "`");

	SetTextColor(color);
	printf(time);

	for (auto& splStr : spl) {
		if (splStr[0] == '~') {
			if (splStr.length() < 2) continue;
			switch (splStr[1]) {
			case '0':
				SetTextColor(ConsoleColor::WHITE);
				break;
			case '1':
				SetTextColor(ConsoleColor::GRAY);
				break;
			case '2':
				SetTextColor(ConsoleColor::DARKGRAY);
				break;
			case '3':
				SetTextColor(ConsoleColor::BLACK);
				break;
			case 'R':
				SetTextColor(ConsoleColor::DARKRED);
				break;
			case 'r':
				SetTextColor(ConsoleColor::RED);
				break;
			case 'Y':
				SetTextColor(ConsoleColor::DARKYELLOW);
				break;
			case 'y':
				SetTextColor(ConsoleColor::YELLOW);
				break;
			case 'G':
				SetTextColor(ConsoleColor::DARKGREEN);
				break;
			case 'g':
				SetTextColor(ConsoleColor::GREEN);
				break;
			case 'M':
				SetTextColor(ConsoleColor::DARKMAGENTA);
				break;
			case 'm':
				SetTextColor(ConsoleColor::MAGENTA);
				break;
			case 'C':
				SetTextColor(ConsoleColor::DARKCYAN);
				break;
			case 'c':
				SetTextColor(ConsoleColor::CYAN);
				break;
			case 'B':
				SetTextColor(ConsoleColor::DARKBLUE);
				break;
			case 'b':
				SetTextColor(ConsoleColor::BLUE);
				break;
			case 'X':
			case 'x':
				SetTextColor(color);
				break;
			default:
				SetTextColor(color);
				break;
			}
			splStr.erase(0, 2);
		}
		printf(splStr.c_str());
		final += splStr;
	}
	printf("\n");

	char buffer2[512] = { 0 };
	sprintf_s(buffer2, "%s%s %s\n", time, type, final.c_str());
	LogToFile(buffer2);

    SetTextColor(ConsoleColor::WHITE);
}

void Logger::LogToFile(const char * buff) {
	// #Dirty add proper path
	const auto fileName = "D:/Development/C++/Emerald/log.log";
	ofstream logFile;
	logFile.open(fileName, ios_base::app);
	if (m_firstEntry) {
		ofstream ofs;
		ofs.open(fileName, ofstream::out | ofstream::trunc);
		ofs.close();
		logFile << endl;
		m_firstEntry = false;
	}
	logFile << buff;
}

void Logger::Cleanup() {
	if (!m_allocated) return;
	LOG_PRINT("[~gLogging] Deallocating console");
	PostMessage(GetConsoleWindow(), WM_CLOSE, 0, 0);
}
