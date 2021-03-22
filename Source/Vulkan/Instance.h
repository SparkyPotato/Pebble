#pragma once

#define VK_NO_PROTOTYPES
#include "vulkan/vulkan.h"
#include "GLFW/glfw3.h"
#include "vk_mem_alloc.h"
#include "volk.h"

class CommandBuffer;
class Fence;
class Semaphore;

namespace Instance {

void Init();
void Cleanup();

VkInstance Instance();
VkDevice Device();
VkPhysicalDevice PhysicalDevice();
VmaAllocator Allocator();

u32 GraphicsIndex();
VkQueue GraphicsQueue();

void WaitForIdle();
void Submit(std::span<CommandBuffer*> buffers, std::span<std::pair<const Semaphore*, VkPipelineStageFlags>> wait,
	std::span<const Semaphore*> signal, const Fence* notify);

extern bool IsInitialized;

};

void HandleVkResult(VkResult result, const char* statement, const char* file, int line);

#ifndef NDEBUG

#	define VkCall(statement)                                                                                          \
		do                                                                                                             \
		{                                                                                                              \
			VkResult result = (statement);                                                                             \
			HandleVkResult(result, #statement, __FILE__, __LINE__);                                                    \
		} while (false)
#else

#	define VkCall(statement) statement

#endif
