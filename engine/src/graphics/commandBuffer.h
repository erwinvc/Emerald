#pragma once

namespace emerald {
	using Command = std::function<void()>;

	class CommandBuffer {
	private:
		std::mutex m_mutex;
		std::vector<Command> m_commands;

	public:
		void pushCommand(Command command);
		void executeCommands();
	};
}