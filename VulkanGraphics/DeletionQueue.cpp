#include "DeletionQueue.h"

void DeletionQueue::Push(std::function<void()>&& function)
{
	m_deletors.push_back(function);
}

void DeletionQueue::Flush()
{
	for (auto it = m_deletors.rbegin(); it != m_deletors.rend(); it++)
		(*it)();

	m_deletors.clear();
}
