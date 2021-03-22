#include "PCH.h"

#include "Sync.h"

Semaphore::Semaphore()
{
	VkSemaphoreCreateInfo info{ .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };

	VkCall(vkCreateSemaphore(Instance::Device(), &info, nullptr, &m_Semaphore));
}

Semaphore::~Semaphore() { vkDestroySemaphore(Instance::Device(), m_Semaphore, nullptr); }

Semaphore::Semaphore(Semaphore&& other)
{
	m_Semaphore = other.m_Semaphore;
	other.m_Semaphore = VK_NULL_HANDLE;
}

Semaphore& Semaphore::operator=(Semaphore&& other)
{
	this->~Semaphore();

	m_Semaphore = other.m_Semaphore;
	other.m_Semaphore = VK_NULL_HANDLE;

	return *this;
}

Fence::Fence(VkFenceCreateFlags flags)
{
	VkFenceCreateInfo info{ .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO, .flags = flags };

	VkCall(vkCreateFence(Instance::Device(), &info, nullptr, &m_Fence));
}

Fence::~Fence() { vkDestroyFence(Instance::Device(), m_Fence, nullptr); }

Fence::Fence(Fence&& other)
{
	m_Fence = other.m_Fence;
	other.m_Fence = VK_NULL_HANDLE;
}

Fence& Fence::operator=(Fence&& other)
{
	this->~Fence();

	m_Fence = other.m_Fence;
	other.m_Fence = VK_NULL_HANDLE;

	return *this;
}

bool Fence::WaitOn(u64 timeout)
{
	return vkWaitForFences(Instance::Device(), 1, &m_Fence, VK_TRUE, timeout) == VK_SUCCESS;
}

void Fence::Reset() { vkResetFences(Instance::Device(), 1, &m_Fence); }
