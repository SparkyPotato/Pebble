#pragma once

#include "Instance.h"

class Buffer
{
public:
	Buffer() = default;
	Buffer(u64 size, VkBufferUsageFlags usage, VmaMemoryUsage memUsage, VkBufferCreateFlags flags = 0);
	~Buffer();

	Buffer(const Buffer& other) = delete;
	Buffer& operator=(const Buffer& other) = delete;

	Buffer(Buffer&& other);
	Buffer& operator=(Buffer&& other);

	void* Map();
	void Unmap();

	void Flush(u64 offset, u64 size);
	void Pull(u64 offset, u64 size);

	VkBuffer GetHandle() const { return m_Buffer; }
	VmaAllocation GetMemory() const { return m_Memory; }

private:
	VkBuffer m_Buffer = VK_NULL_HANDLE;
	VmaAllocation m_Memory = VK_NULL_HANDLE;
};
