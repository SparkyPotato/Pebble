#pragma once

#define VOLK_VULKAN_H_PATH "vulkan/vulkan.h"
#include "volk.h"
#include "vk_mem_alloc.h"
#include "GLFW/glfw3.h"

namespace Instance {

void Init();
void Cleanup();

VkInstance Instance();
VkDevice Device();
VkPhysicalDevice PhysicalDevice();
VmaAllocator Allocator();

u32 GraphicsIndex();
u32 TransferIndex();

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
