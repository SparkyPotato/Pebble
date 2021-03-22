#pragma once

#include "Instance.h"

class Semaphore
{
public:
	Semaphore();
	~Semaphore();

	Semaphore(const Semaphore& other) = delete;
	Semaphore& operator=(const Semaphore& other) = delete;

	Semaphore(Semaphore&& other);
	Semaphore& operator=(Semaphore&& other);

	VkSemaphore GetHandle() const { return m_Semaphore; }

private:
	VkSemaphore m_Semaphore = VK_NULL_HANDLE;
};

class Fence
{
public:
	Fence(VkFenceCreateFlags flags = 0);
	~Fence();

	Fence(const Fence& other) = delete;
	Fence& operator=(const Fence& other) = delete;

	Fence(Fence&& other);
	Fence& operator=(Fence&& other);

	VkFence GetHandle() const { return m_Fence; }

	bool WaitOn(u64 timeout = -1);
	void Reset();

private:
	VkFence m_Fence = VK_NULL_HANDLE;
};
