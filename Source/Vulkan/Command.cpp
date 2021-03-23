#include "PCH.h"

#include "Command.h"

#include "Buffer.h"
#include "Description.h"
#include "Descriptor.h"
#include "Framebuffer.h"
#include "Pipeline.h"
#include "PipelineLayout.h"

CommandBuffer::CommandBuffer(VkCommandPool pool, VkCommandBufferLevel level) : m_Pool(pool)
{
	VkCommandBufferAllocateInfo info{ .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.commandPool = pool,
		.level = level,
		.commandBufferCount = 1 };

	VkCall(vkAllocateCommandBuffers(Instance::Device(), &info, &m_Buffer));
}

void CommandBuffer::Begin(VkCommandBufferUsageFlags flags, std::optional<InheritanceInfo> inInfo)
{
	VkCommandBufferBeginInfo info{ .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO, .flags = flags };
	VkCommandBufferInheritanceInfo iInfo{};
	if (inInfo)
	{
		const InheritanceInfo& iinfo = inInfo.value();
		iInfo.renderPass = iinfo.Pass->GetHandle();
		iInfo.subpass = iinfo.SubpassIndex;
		iInfo.framebuffer = iinfo.Framebuf->GetHandle();
		info.pInheritanceInfo = &iInfo;
	}

	VkCall(vkBeginCommandBuffer(m_Buffer, &info));
}

void CommandBuffer::End() { VkCall(vkEndCommandBuffer(m_Buffer)); }

void CommandBuffer::BeginRenderPass(const RenderPass& renderPass, const Framebuffer& framebuffer, VkRect2D area,
	std::span<VkClearValue> clearValues, VkSubpassContents contents)
{
	VkRenderPassBeginInfo info{ .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
		.renderPass = renderPass.GetHandle(),
		.framebuffer = framebuffer.GetHandle(),
		.renderArea = area,
		.clearValueCount = u32(clearValues.size()),
		.pClearValues = clearValues.data() };

	vkCmdBeginRenderPass(m_Buffer, &info, contents);
}

void CommandBuffer::EndRenderPass() { vkCmdEndRenderPass(m_Buffer); }

void CommandBuffer::BindPipeline(const Pipeline& pipeline)
{
	vkCmdBindPipeline(m_Buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.GetHandle());
}

void CommandBuffer::BindViewport(const Viewport& viewport)
{
	vkCmdSetViewport(m_Buffer, 0, 1, &viewport.GetViewport());
	vkCmdSetScissor(m_Buffer, 0, 1, &viewport.GetScissor());
}

void CommandBuffer::BindVertexBuffer(const Buffer& buffer, u64 offset)
{
	VkBuffer buf = buffer.GetHandle();
	vkCmdBindVertexBuffers(m_Buffer, 0, 1, &buf, &offset);
}

void CommandBuffer::BindIndexBuffer(const Buffer& buffer, u64 offset, VkIndexType type)
{
	vkCmdBindIndexBuffer(m_Buffer, buffer.GetHandle(), offset, type);
}

void CommandBuffer::BindDescriptorSet(
	const PipelineLayout& layout, u32 index, const DescriptorSet& set, std::optional<u32> dynamicOffset)
{
	VkDescriptorSet s = set.GetHandle();

	vkCmdBindDescriptorSets(m_Buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, layout.GetHandle(), index, 1, &s,
		dynamicOffset ? 0 : 1, &dynamicOffset.value());
}

void CommandBuffer::CopyBuffer(const Buffer& from, const Buffer& to, std::span<VkBufferCopy> regions)
{
	vkCmdCopyBuffer(m_Buffer, from.GetHandle(), to.GetHandle(), u32(regions.size()), regions.data());
}

void CommandBuffer::Draw(u32 vertexCount, u32 instanceCount, u32 firstVertex, u32 firstInstance)
{
	vkCmdDraw(m_Buffer, vertexCount, instanceCount, firstVertex, firstInstance);
}

void CommandBuffer::DrawIndexed(u32 indexCount, u32 instanceCount, u32 firstIndex, i32 vertexOffset, u32 firstInstance)
{
	vkCmdDrawIndexed(m_Buffer, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
}

CommandBuffer::~CommandBuffer()
{
	if (m_Pool)
	{
		vkFreeCommandBuffers(Instance::Device(), m_Pool, 1, &m_Buffer);
	}
}

CommandBuffer::CommandBuffer(CommandBuffer&& other)
{
	m_Buffer = other.m_Buffer;
	other.m_Buffer = VK_NULL_HANDLE;
	m_Pool = other.m_Pool;
}

CommandBuffer& CommandBuffer::operator=(CommandBuffer&& other)
{
	this->~CommandBuffer();

	m_Buffer = other.m_Buffer;
	other.m_Buffer = VK_NULL_HANDLE;
	m_Pool = other.m_Pool;

	return *this;
}

CommandPool::CommandPool(VkCommandPoolCreateFlags flags)
{
	VkCommandPoolCreateInfo info{ .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		.flags = flags,
		.queueFamilyIndex = Instance::GraphicsIndex() };

	VkCall(vkCreateCommandPool(Instance::Device(), &info, nullptr, &m_Pool));
}

CommandBuffer CommandPool::Allocate(VkCommandBufferLevel level) { return CommandBuffer(m_Pool, level); }

CommandPool::~CommandPool() { vkDestroyCommandPool(Instance::Device(), m_Pool, nullptr); }

CommandPool::CommandPool(CommandPool&& other)
{
	m_Pool = other.m_Pool;
	other.m_Pool = VK_NULL_HANDLE;
}

CommandPool& CommandPool::operator=(CommandPool&& other)
{
	this->~CommandPool();

	m_Pool = other.m_Pool;
	other.m_Pool = VK_NULL_HANDLE;

	return *this;
}
