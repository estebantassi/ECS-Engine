#include <vector>
#include <functional>

class CommandBuffer
{
public:
	void QueueCommand(std::function<void()> command) { m_PendingCommands.push_back(command); }

	void RunCommands() {
		for (auto& command : m_PendingCommands)
			command();
		m_PendingCommands.clear();
	}

private:
	std::vector<std::function<void()>> m_PendingCommands;
};