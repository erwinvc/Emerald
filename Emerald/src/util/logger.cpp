#include "stdafx.h"

/*Non-blocking logger with color formatting*/

bool Logger::m_allocated = false;
bool Logger::m_firstEntry = true;
bool Logger::m_stopping = false;

HANDLE Logger::m_outputHandle = INVALID_HANDLE_VALUE;
HANDLE Logger::m_inputHandle = INVALID_HANDLE_VALUE;
Thread* Logger::m_inputThread;
Thread* Logger::m_outputThread;
CONSOLE_SCREEN_BUFFER_INFO Logger::m_screenBuffer;

mutex Logger::m_mutex;
condition_variable Logger::m_conditionVariable;
queue<Logger::QueuedMessage> Logger::m_queue;

void HandleInput() {
	static String input;
	getline(cin, input);
	LOG("~R%s", input.c_str());
}

/*Retrieve QueuedMessages from the queue (async)*/
void Logger::HandleQueue() {
	static QueuedMessage message;
	{
		std::unique_lock <mutex> lock(m_mutex);
		while (m_queue.empty()) m_conditionVariable.wait(lock);
		if (m_queue.empty())return;
		message = move(m_queue.front());
		m_queue.pop();
	}
	ProcessMessage(message);
}

/*Initialize the non-blocking logger*/
void Logger::Initialize() {
	if (m_allocated) return;

	AllocConsole();
	SetConsoleTitleA("Emerald");

	m_outputHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	m_inputHandle = GetStdHandle(STD_INPUT_HANDLE);
	GetConsoleScreenBufferInfo(m_outputHandle, &m_screenBuffer);

	const int width = 120;
	const int height = 30;

	m_allocated = true;
	m_firstEntry = true;

	m_inputThread = GetThreadManager()->RegisterThread("Console input", HandleInput);
	m_outputThread = GetThreadManager()->RegisterThread("Console output", HandleQueue);

	LOG("[~2Logging~x] Console allocated");
}

/*Set the text color of the next print to console*/
void Logger::SetTextColor(const int color) {
	WORD attributes = m_screenBuffer.wAttributes & ~FOREGROUND_RED & ~FOREGROUND_GREEN & ~FOREGROUND_BLUE & ~FOREGROUND_INTENSITY;
	attributes |= color;
	SetConsoleTextAttribute(m_outputHandle, attributes);
}

/*Get the time as a printable string*/
const char* Logger::GetTimeAsString(time_t& currentTime) {
	struct tm timeStruct;
	localtime_s(&timeStruct, &currentTime);

	static char buffer[48];
	sprintf_s(buffer, "[%02d:%02d:%02d]", timeStruct.tm_hour, timeStruct.tm_min, timeStruct.tm_sec);
	return buffer;
}

/*Add QueuedMessage to the async queue*/
void Logger::AddToQueue(int color, const String& message, const String& type, time_t time) {
	unique_lock<mutex> l(m_mutex);
	m_queue.emplace(move(QueuedMessage(color, message, type, time)));
	m_conditionVariable.notify_one();
}

/*Process a QueuedMessage (async)*/
void Logger::ProcessMessage(QueuedMessage& message) {
	String_t formattedTime = GetTimeAsString(message.m_time);
	String outputString;
	String formattedMessage = Utils::ReplaceString(message.m_message, "~", "`~");
	vector<String> splitMessage = Utils::Split(formattedMessage, "`");

	SetTextColor(message.m_color);
	printf("%s", formattedTime);

	for (auto& splStr : splitMessage) {
		if (splStr[0] == '~') {
			if (splStr.length() < 2) continue;
			switch (splStr[1]) {
			case '0': SetTextColor((int)ConsoleColor::WHITE); break;
			case '1': SetTextColor((int)ConsoleColor::GRAY); break;
			case '2': SetTextColor((int)ConsoleColor::DARKGRAY); break;
			case '3': SetTextColor((int)ConsoleColor::BLACK); break;
			case 'R': SetTextColor((int)ConsoleColor::DARKRED); break;
			case 'r': SetTextColor((int)ConsoleColor::RED); break;
			case 'Y': SetTextColor((int)ConsoleColor::DARKYELLOW); break;
			case 'y': SetTextColor((int)ConsoleColor::YELLOW); break;
			case 'G': SetTextColor((int)ConsoleColor::DARKGREEN); break;
			case 'g': SetTextColor((int)ConsoleColor::GREEN); break;
			case 'M': SetTextColor((int)ConsoleColor::DARKMAGENTA); break;
			case 'm': SetTextColor((int)ConsoleColor::MAGENTA); break;
			case 'C': SetTextColor((int)ConsoleColor::DARKCYAN); break;
			case 'c': SetTextColor((int)ConsoleColor::CYAN); break;
			case 'B': SetTextColor((int)ConsoleColor::DARKBLUE); break;
			case 'b': SetTextColor((int)ConsoleColor::BLUE); break;
			case 'X':
			case 'x':SetTextColor(message.m_color); break;
			default:SetTextColor(message.m_color); break;
			}
			splStr.erase(0, 2);
		}
		printf("%s", splStr.c_str());
		outputString += splStr;
	}
	printf("\n");

	char buffer2[0xffff] = { 0 };
	sprintf_s(buffer2, "%s%s %s\n", formattedTime, message.m_type.c_str(), outputString.c_str());
	LogToFile(buffer2);

	SetTextColor((int)ConsoleColor::WHITE);
}

void Logger::Message(int color, const char* type, const char* fmt, ...) {
	if (m_stopping) return;
	if (m_queue.size() > MAXQUEUESIZE) {
		LOG_ERROR("[~2Logging~x] Console queue size exceeded 100. Too many messages are being sent");
		return;
	}
	char buffer[0xffff] = { 0 };
	va_list va_alist;

	va_start(va_alist, fmt);
	vsprintf_s(buffer, fmt, va_alist);
	va_end(va_alist);


	AddToQueue(color, buffer, type, time(nullptr));
}
void Logger::MessageTimed(int time, int color, const char* type, const char* fmt, ...) {
	if (GetApp()->GetFrameCount() % time == 0) {
		char buffer[0xffff] = { 0 };
		va_list va_alist;
		va_start(va_alist, fmt);
		vsprintf_s(buffer, fmt, va_alist);
		va_end(va_alist);

		String str = buffer;
		str += to_string(GetApp()->GetFrameCount());
		Message(color, type, str.c_str());
	}
}
void Logger::MessageDirect(int color, const char* type, const char* fmt, ...) {
	if (m_stopping) return;
	m_stopping = true;
	char buffer[0xffff] = { 0 };
	va_list va_alist;

	va_start(va_alist, fmt);
	vsprintf_s(buffer, fmt, va_alist);
	va_end(va_alist);

	AddToQueue(color, buffer, type, time(nullptr));

	ForceEmptyQueue();
	__debugbreak();
}

void Logger::ForceEmptyQueue() {
	m_outputThread->Shutdown();

	while (!m_outputThread->IsFinished()) Sleep(0);

	while (!m_queue.empty()) {
		ProcessMessage(move(m_queue.front()));
		m_queue.pop();
	}
}

/*Print the message to a logging file*/
void Logger::LogToFile(const char * buff) {
	// #Dirty add proper path
	const auto fileName = "log.log";
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
	LOG("[~gLogging~x] Deallocating console");
	ForceEmptyQueue();
	PostMessage(GetConsoleWindow(), WM_CLOSE, 0, 0);
}
