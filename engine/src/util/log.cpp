#include "eepch.h"
#include "datastructures/asyncQueue.h"
#include "log.h"
#include "threading/threadManager.h"
#include <application.h>
#include <format>
#include <iostream>
#include <sstream>
#include <chrono>

namespace emerald {
	bool Log::s_initialized = false;
	bool Log::s_shutdown = false;
	LogLevel Log::s_currentLogLevel = LogLevel::INFO;
	std::string Log::s_logPath = "log.log";
	std::ofstream Log::m_logFile;

	HANDLE Log::s_outputHandle = INVALID_HANDLE_VALUE;
	Thread* Log::s_outputThread;
	CONSOLE_SCREEN_BUFFER_INFO Log::s_screenBuffer;

	AsyncQueue<Log::QueuedMessage> Log::m_queue;
	/*Retrieve QueuedMessages from the queue (async)*/
	void Log::handleQueue() {
		while (!s_shutdown) {
			static QueuedMessage message;
			if (m_queue.waitForGet(message)) {
				processMessage(message);
			}
		}
	}

	/*Initialize the non-blocking logger*/
	void Log::initialize(const char* title) {
		if (s_initialized) return;
		m_logFile.open(s_logPath, std::ios_base::app);

		AllocConsole();
		SetConsoleTitleA(title);

		s_outputHandle = GetStdHandle(STD_OUTPUT_HANDLE);
		GetConsoleScreenBufferInfo(s_outputHandle, &s_screenBuffer);

		const int width = 120;
		const int height = 30;

		s_initialized = true;

		Log::info("[Console] allocated");

		s_outputThread = ThreadManager::createAndRegisterThread(ThreadType::CONSOLE_OUTPUT, "Console output", handleQueue);

		std::ofstream ofs;
		ofs.open(s_logPath, std::ofstream::out | std::ofstream::trunc);
		ofs.close();
	}

	/*Set the text color of the next print to console*/
	void Log::setTextColor(ConsoleColor color) {
		WORD attributes = s_screenBuffer.wAttributes & ~FOREGROUND_RED & ~FOREGROUND_GREEN & ~FOREGROUND_BLUE & ~FOREGROUND_INTENSITY;
		attributes |= (int)color;
		SetConsoleTextAttribute(s_outputHandle, attributes);
	}

	/*Get the time as a printable string*/
	std::string Log::getTimeAsString(std::chrono::system_clock::time_point currentTime) {
		auto currentTimeT = std::chrono::system_clock::to_time_t(currentTime);

		std::tm timeStruct{};
		localtime_s(&timeStruct, &currentTimeT); // Safely convert time_t to tm struct

		std::ostringstream ss;
		ss << std::put_time(&timeStruct, "[%H:%M:%S]"); // Use std::put_time for thread-safe time formatting

		return ss.str(); // Return the formatted time string
	}

	/*Process a QueuedMessage (async)*/
	void Log::processMessage(const QueuedMessage& message) {
		std::string formattedTime = getTimeAsString(message.m_time);

		std::ostringstream ss;
		ss << getTimeAsString(message.m_time) << message.m_type << " " << message.m_message << "\n";

		setTextColor(message.m_color);
		printf("%s", ss.str().c_str());

		if (m_logFile) m_logFile << ss.str();

		setTextColor(ConsoleColor::WHITE);
	}

	void Log::logMessage(LogLevel level, const std::string& type, const std::string& message) {
		if (s_shutdown) return;
		if (level > s_currentLogLevel) return;
		ConsoleColor color = ConsoleColor::WHITE; // Default color
		switch (level) {
			case LogLevel::FAIL: color = ConsoleColor::RED; break;
			case LogLevel::WARN: color = ConsoleColor::YELLOW; break;
			case LogLevel::INFO: color = ConsoleColor::WHITE; break;
		}
		m_queue.add(std::move(QueuedMessage(color, message, type, std::chrono::system_clock::now())));
	}

	void Log::forceEmptyQueue() {
		s_shutdown = true;

		m_queue.shutdown();
		s_outputThread->shutdown();

		QueuedMessage message;
		while (m_queue.tryToGet(message)) {
			processMessage(message);
		}
	}

	void Log::cleanup() {
		Log::info("[Console] deallocating...");
		if (m_logFile.is_open()) m_logFile.close();
		if (!s_initialized) return;
		forceEmptyQueue();
		PostMessage(GetConsoleWindow(), WM_CLOSE, 0, 0);
	}
}