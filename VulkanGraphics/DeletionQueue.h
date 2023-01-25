#include <functional>
#include <deque>

class DeletionQueue
{
private:
	std::deque<std::function<void()>> m_deletors;
public:
	void Push(std::function<void()>&& function);
	void Flush();
};