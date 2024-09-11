#include "eepch.h"
#include "datastructures/asyncQueue.h"
#include "log.h"
#include "threading/threadManager.h"
#include <application.h>
#include <format>
#include <iostream>
#include <sstream>
#include <chrono>
#include "debug.h"

namespace emerald {
	static std::mutex s_messageMutex;
	static HANDLE s_outputHandle = INVALID_HANDLE_VALUE;
	static std::string s_logPath = "log.log";
	static std::ofstream s_logFile;
	static Thread* s_outputThread;
	static CONSOLE_SCREEN_BUFFER_INFO s_screenBuffer;
	static AsyncQueue<Log::QueuedMessage> s_messageQueue;
	static std::vector<LogMessage> s_messages;

	static bool s_initialized;
	static bool s_shutdown;

	/*Retrieve QueuedMessages from the queue (async)*/
	void Log::handleQueue() {
		while (!s_shutdown) {
			static QueuedMessage message;
			if (s_messageQueue.waitForGet(message)) {
				processMessage(message);
			}
		}
	}

	/*Initialize the non-blocking logger*/
	void Log::initialize(const char* title) {
		if (s_initialized) return;
		s_logFile.open(s_logPath, std::ios_base::app);

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
		static const char* LogLevelStrings[] = { "FATAL", "ERROR", "WARN", "INFO" };

		std::string formattedTime = getTimeAsString(message.m_time);

		std::ostringstream ss;
		ss << getTimeAsString(message.m_time) << LogLevelStrings[(uint32_t)message.m_level] << " " << message.m_message << "\n";

		setTextColor(message.m_color);
		printf("%s", ss.str().c_str());

		if (s_logFile) s_logFile << ss.str();

		{
			std::lock_guard<std::mutex> lock(s_messageMutex);  // Lock the mutex to protect the vector
			s_messages.emplace_back(LogMessage{ message.m_message, message.m_stackTrace, formattedTime, message.m_level });
		}

		setTextColor(ConsoleColor::WHITE);
	}

	void Log::logMessage(LogLevel level, const std::string& message) {
		if (s_shutdown) return;

		ConsoleColor color = ConsoleColor::WHITE; // Default color
		switch (level) {
			case LogLevel::ERROR: color = ConsoleColor::RED; break;
			case LogLevel::WARN: color = ConsoleColor::YELLOW; break;
			case LogLevel::INFO: color = ConsoleColor::WHITE; break;
		}
		s_messageQueue.add(std::move(QueuedMessage(color, message, Debug::captureStackTrace(3, 64, false), level, std::chrono::system_clock::now())));
	}

	void Log::forceEmptyQueue() {
		s_shutdown = true;

		s_messageQueue.shutdown();
		s_outputThread->shutdown();

		QueuedMessage message;
		while (s_messageQueue.tryToGet(message)) {
			processMessage(message);
		}
	}

	void Log::shutdown() {
		Log::info("[Console] deallocating...");
		if (s_logFile.is_open()) s_logFile.close();
		if (!s_initialized) return;
		forceEmptyQueue();
		PostMessage(GetConsoleWindow(), WM_CLOSE, 0, 0);
	}

	std::vector<LogMessage>& Log::getMessages() {
		return s_messages;
	}

	void Log::clearMessages() {
		std::lock_guard<std::mutex> lock(s_messageMutex);
		s_messages.clear();
	}

	std::mutex& Log::getMessageMutex() {
		return s_messageMutex;
	}
}