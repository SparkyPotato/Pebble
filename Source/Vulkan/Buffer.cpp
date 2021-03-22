#include "PCH.h"

#include "Buffer.h"

Buffer::Buffer(u64 size, VkBufferUsageFlags usage, VmaMemoryUsage memUsage, VkBufferCreateFlags flags)
{
	u32 index = Instance::GraphicsIndex();

	VkBufferCreateInfo info{ .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		.flags = flags,
		.size = size,
		.usage = usage,
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
		.queueFamilyIndexCount = 1,
		.pQueueFamilyIndices = &index };

	VmaAllocationCreateInfo allocInfo{ .usage = memUsage };

	VkCall(vmaCreateBuffer(Instance::Allocator(), &info, &allocInfo, &m_Buffer, &m_Memory, nullptr));
}

void* Buffer::Map()
{
	void* data;
	VkCall(vmaMapMemory(Instance::Allocator(), m_Memory, &data));
	return data;
}

void Buffer::Unmap() { vmaUnmapMemory(Instance::Allocator(), m_Memory); }

void Buffer::Flush(u64 offset, u64 size) { vmaFlushAllocation(Instance::Allocator(), m_Memory, offset, size); }

void Buffer::Pull(u64 offset, u64 size) { vmaInvalidateAllocation(Instance::Allocator(), m_Memory, offset, size); }

Buffer::~Buffer() { vmaDestroyBuffer(Instance::Allocator(), m_Buffer, m_Memory); }

Buffer::Buffer(Buffer&& other)
{
	m_Buffer = other.m_Buffer;
	other.m_Buffer = VK_NULL_HANDLE;
	m_Memory = other.m_Memory;
	other.m_Memory = VK_NULL_HANDLE;
}

Buffer& Buffer::operator=(Buffer&& other)
{
	this->~Buffer();

	m_Buffer = other.m_Buffer;
	other.m_Buffer = VK_NULL_HANDLE;
	m_Memory = other.m_Memory;
	other.m_Memory = VK_NULL_HANDLE;

	return *this;
}
