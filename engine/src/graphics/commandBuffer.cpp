#include "eepch.h"
#include "commandBuffer.h"

namespace emerald {
	void CommandBuffer::pushCommand(Command command) {
		std::lock_guard<std::mutex> lock(m_mutex);
		m_commands.push_back(command);
	}

	void CommandBuffer::executeCommands() {
		std::unique_lock<std::mutex> lock(m_mutex);
		std::vector<Command> commands = std::move(m_commands);
		lock.unlock();

		for (const auto& command : commands) {
			command();
		}

		//Log::info("Executed {} commands", commands.size());
	}
}