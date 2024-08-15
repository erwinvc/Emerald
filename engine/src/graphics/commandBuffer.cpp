#include "eepch.h"
#include "commandBuffer.h"

namespace emerald {
	void CommandBuffer::pushCommand(Command command) {
        std::lock_guard<std::mutex> lock(m_commandsMutex);
        //		ASSERT(!m_executingCommands, "Cannot push command while executing commands");
		m_commands.push_back(command);
	}

	void CommandBuffer::executeCommands() {
        std::vector<Command> commands;
        {
            std::lock_guard<std::mutex> lock(m_commandsMutex);
			//if (m_commands.empty()) {
			//    Log::info("Empty");
			//}
            commands = std::move(m_commands);
        }

        std::unique_lock<std::mutex> lock(m_executingMutex);
        m_executingCommands = true;
        lock.unlock();

        for (const auto& command : commands) {
            command();
        }

        lock.lock();
        m_executingCommands = false;
        lock.unlock();
	}
}