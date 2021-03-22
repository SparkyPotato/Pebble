#pragma once

#include "Instance.h"

class Buffer;
class RenderPass;
class Framebuffer;
class Pipeline;
class Viewport;

struct InheritanceInfo
{
	RenderPass* Pass;
	u32 SubpassIndex;
	Framebuffer* Framebuf;
};

class CommandBuffer
{
public:
	CommandBuffer() = default;
	~CommandBuffer();

	CommandBuffer(const CommandBuffer& other) = delete;
	CommandBuffer& operator=(const CommandBuffer& other) = delete;

	CommandBuffer(CommandBuffer&& other);
	CommandBuffer& operator=(CommandBuffer&& other);

	VkCommandBuffer GetHandle() const { return m_Buffer; }

	void Begin(VkCommandBufferUsageFlags flags = 0, std::optional<InheritanceInfo> info = std::nullopt);
	void End();

	void BeginRenderPass(const RenderPass& renderPass, const Framebuffer& framebuffer, VkRect2D area,
		std::span<VkClearValue> clearValues, VkSubpassContents contents = VK_SUBPASS_CONTENTS_INLINE);
	void EndRenderPass();

	void BindPipeline(const Pipeline& pipeline);
	void BindViewport(const Viewport& viewport);
	void BindVertexBuffer(const Buffer& buffer, u64 offset);

	void Draw(u32 vertexCount, u32 instanceCount, u32 firstVertex, u32 firstInstance);

private:
	friend class CommandPool;

	CommandBuffer(VkCommandPool pool, VkCommandBufferLevel level);

	VkCommandBuffer m_Buffer = VK_NULL_HANDLE;
	VkCommandPool m_Pool = VK_NULL_HANDLE;
};

class CommandPool
{
public:
	CommandPool(VkCommandPoolCreateFlags flags = 0);
	~CommandPool();

	CommandPool(const CommandPool& other) = delete;
	CommandPool& operator=(const CommandPool& other) = delete;

	CommandPool(CommandPool&& other);
	CommandPool& operator=(CommandPool&& other);

	VkCommandPool GetHandle() const { return m_Pool; }

	CommandBuffer Allocate(VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);

private:
	VkCommandPool m_Pool = VK_NULL_HANDLE;
};
