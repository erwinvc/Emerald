#pragma once

namespace emerald {
	using Command = std::function<void()>;

	class CommandBuffer {
	private:
		bool m_executingCommands = false;
		std::mutex m_commandsMutex;
		std::mutex m_executingMutex;
		std::vector<Command> m_commands;

	public:
		void pushCommand(Command command);
		void executeCommands();
	};
}