#include "eepch.h"
#include "core/application//application.h"
#include "log.h"
#include "utils/datastructures/asyncQueue.h"
#include "utils/debug/debug.h"
#include "utils/threading/threadManager.h"
#include <chrono>
#include <format>
#include <iostream>
#include <sstream>

namespace emerald {
	static std::mutex s_messageMutex;
	static HANDLE s_outputHandle = INVALID_HANDLE_VALUE;
	static std::string s_logPath = "log.log";
	static std::ofstream s_logFile;
	static Thread* s_outputThread;
	static CONSOLE_SCREEN_BUFFER_INFO s_screenBuffer;
	static AsyncQueue<Log::QueuedMessage> s_messageQueue;
	static std::vector<LogMessage> s_messages;
	static ConsoleColor s_currentConsoleColor = ConsoleColor::RED;

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

		setTextColor(ConsoleColor::WHITE);

		// Disable QuickEdit Mode
		HANDLE inputHandle = GetStdHandle(STD_INPUT_HANDLE);
		DWORD mode = 0;
		if (!GetConsoleMode(inputHandle, &mode)) {
			Log::error("[Console] failed to get console mode");
		}
		mode &= ~ENABLE_QUICK_EDIT_MODE;
		mode |= ENABLE_EXTENDED_FLAGS;
		if (!SetConsoleMode(inputHandle, mode)) {
			Log::error("[Console] failed to set console mode");
		}

		s_outputThread = ThreadManager::createAndRegisterThread(ThreadType::CONSOLE_OUTPUT, "Console output", handleQueue);

		std::ofstream ofs;
		ofs.open(s_logPath, std::ofstream::out | std::ofstream::trunc);
		ofs.close();

		s_initialized = true;

		Log::info("[Console] allocated");
	}

	/*Set the text color of the next print to console*/
	void Log::setTextColor(ConsoleColor color) {
		if (s_currentConsoleColor == color) return;
		WORD attributes = s_screenBuffer.wAttributes & ~FOREGROUND_RED & ~FOREGROUND_GREEN & ~FOREGROUND_BLUE & ~FOREGROUND_INTENSITY;
		attributes |= (int)color;
		SetConsoleTextAttribute(s_outputHandle, attributes);
		s_currentConsoleColor = color;
	}

	/*Get the time as a printable string*/
	std::string Log::getTimeAsString(std::chrono::system_clock::time_point currentTime) {
		auto currentTimeT = std::chrono::system_clock::to_time_t(currentTime);
		std::tm localTime;
		localtime_s(&localTime, &currentTimeT);
		char buffer[16];
		std::strftime(buffer, sizeof(buffer), "[%H:%M:%S]", &localTime);
		return buffer;
	}

	/*Process a QueuedMessage (async)*/
	void Log::processMessage(const QueuedMessage& message) {
		static const char* LogLevelStrings[] = { "FATAL", "ERROR", "WARN", "INFO" };

		std::string formattedTime = getTimeAsString(message.m_time);

		std::string output = std::format("{} [{}] {}\n", formattedTime, LogLevelStrings[(uint32_t)message.m_level], message.m_message);

		setTextColor(message.m_color);
		printf("%s", output.c_str());

		if (s_logFile) s_logFile << output;

		{
			std::lock_guard<std::mutex> lock(s_messageMutex);
			s_messages.emplace_back(LogMessage{ message.m_message, message.m_stackTrace, formattedTime, message.m_level });
		}
	}

	void Log::logMessage(Severity level, const std::string& message) {
		if (s_shutdown || s_messageQueue.isReleased()) return;

		ConsoleColor color = ConsoleColor::WHITE; // Default color
		switch (level) {
			case Severity::ERROR: color = ConsoleColor::RED; break;
			case Severity::WARN: color = ConsoleColor::YELLOW; break;
			case Severity::INFO: color = ConsoleColor::WHITE; break;
		}
		s_messageQueue.add(std::move(QueuedMessage(color, message, Debug::captureStackTrace(3, 64, false), level, std::chrono::system_clock::now())));
	}

	void Log::forceEmptyQueue() {
		s_shutdown = true;

		s_messageQueue.release();
		s_outputThread->shutdown();

		QueuedMessage message;
		while (s_messageQueue.tryGet(message)) {
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