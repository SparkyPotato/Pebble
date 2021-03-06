#include "PCH.h"

#include "Instance.h"

#include "GLFW/glfw3.h"

#define WIN32_LEAN_AND_MEAN
#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"

#include "Command.h"
#include "Sync.h"

namespace Instance {

bool IsInitialized = false;

static bool s_ValidationEnabled = false;

VkInstance s_Instance = VK_NULL_HANDLE;
VkDevice s_Device = VK_NULL_HANDLE;
VmaAllocator s_Allocator = VK_NULL_HANDLE;

VkPhysicalDevice s_PhysicalDevice = VK_NULL_HANDLE;

VkDebugUtilsMessengerEXT s_DebugMessenger = VK_NULL_HANDLE;

VkQueue s_GraphicsQueue = VK_NULL_HANDLE;
u32 s_GraphicsQueueIndex;

template<typename Func, typename... Args>
auto LoadAndCall(const char* name, const Args&... args)
{
	auto function = reinterpret_cast<Func>(vkGetInstanceProcAddr(s_Instance, name));
	if (function != nullptr)
	{
		return function(s_Instance, args...);
	}
	else
	{
		CRITICAL("Function '{}' not available", name);
	}
}

#define VkLoad(func, ...) LoadAndCall<PFN_##func>(#func, __VA_ARGS__)

static std::vector<const char*> GetInstanceLayers();
static std::vector<const char*> GetInstanceExtensions();
static void CreateInstance(const std::vector<const char*>& layers, const std::vector<const char*>& extensions);
static void SetupDebugCallback();
static VkPhysicalDevice PickPhysicalDevice();
static void CreateDevice(VkPhysicalDevice phyDevice);

VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT severity,
	VkDebugUtilsMessageTypeFlagsEXT type, const VkDebugUtilsMessengerCallbackDataEXT* callback, void* user);

void Init()
{
	VkCall(volkInitialize());
	auto layers = GetInstanceLayers(); // To ensure that GetInstanceLayers() is called before GetInstanceExtensions()
	CreateInstance(layers, GetInstanceExtensions());
	SetupDebugCallback();
	CreateDevice(PickPhysicalDevice());

	IsInitialized = true;
}

void Cleanup()
{
	IsInitialized = false;

	vmaDestroyAllocator(s_Allocator);
	vkDestroyDevice(s_Device, nullptr);
#ifndef NDEBUG
	VkLoad(vkDestroyDebugUtilsMessengerEXT, s_DebugMessenger, nullptr);
#endif
	vkDestroyInstance(s_Instance, nullptr);
	s_Instance = nullptr;
}

static std::vector<const char*> GetInstanceLayers()
{
	u32 count;
	VkCall(vkEnumerateInstanceLayerProperties(&count, nullptr));
	std::vector<VkLayerProperties> availableLayerProperties(count);
	VkCall(vkEnumerateInstanceLayerProperties(&count, availableLayerProperties.data()));
	std::vector<const char*> layers;

#ifndef NDEBUG
	TRACE("Available layers:");
#endif
	for (const auto& layer : availableLayerProperties)
	{
#ifndef NDEBUG
		TRACE("{}: {}", layer.layerName, layer.description);
#endif

#ifndef NDEBUG
		if (std::strcmp(layer.layerName, "VK_LAYER_KHRONOS_validation") == 0)
		{
			s_ValidationEnabled = true;
			layers.push_back("VK_LAYER_KHRONOS_validation");
		}
#endif
	}

#ifndef NDEBUG
	if (!s_ValidationEnabled)
	{
		WARN("Validation layer not supported by this implementation, will not get any validation messages. You might "
			 "need to install the Vulkan SDK.");
	}
	else
	{
		DEBUG("Validation layer enabled");
	}
#endif

	return layers;
}

static std::vector<const char*> GetInstanceExtensions()
{
	u32 count;
	auto data = glfwGetRequiredInstanceExtensions(&count);
	std::vector<const char*> extensions(data, data + count);
	if (s_ValidationEnabled)
	{
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	return extensions;
}

static void CreateInstance(const std::vector<const char*>& layers, const std::vector<const char*>& extensions)
{
	VkApplicationInfo appInfo{ .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		.pApplicationName = "Pebble",
		.applicationVersion = VK_MAKE_VERSION(0, 0, 1),
		.pEngineName = "Pebble",
		.engineVersion = VK_MAKE_VERSION(0, 0, 1),
		.apiVersion = VK_API_VERSION_1_1 };

	VkInstanceCreateInfo info{ .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.pApplicationInfo = &appInfo,
		.enabledLayerCount = u32(layers.size()),
		.ppEnabledLayerNames = layers.data(),
		.enabledExtensionCount = u32(extensions.size()),
		.ppEnabledExtensionNames = extensions.data() };

	VkCall(vkCreateInstance(&info, nullptr, &s_Instance));
	volkLoadInstanceOnly(s_Instance);
	TRACE("Create Vulkan Instance");
}

void SetupDebugCallback()
{
	if (!s_ValidationEnabled)
	{
		return;
	}

	VkDebugUtilsMessengerCreateInfoEXT info{ .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
		.pNext = nullptr,
		.messageSeverity =
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT
			| VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
		.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
					   | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
		.pfnUserCallback = DebugCallback,
		.pUserData = nullptr };

	VkCall(VkLoad(vkCreateDebugUtilsMessengerEXT, &info, nullptr, &s_DebugMessenger));
}

struct QueueFamilyIndices
{
	std::optional<u32> Graphics;

	bool IsComplete() { return Graphics.has_value(); }

	std::optional<u32>* begin() { return &Graphics; }
	std::optional<u32>* end() { return &Graphics + 1; }
};

QueueFamilyIndices GetQueueFamilies(VkPhysicalDevice device)
{
	QueueFamilyIndices indices;
	u32 count;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &count, nullptr);
	std::vector<VkQueueFamilyProperties> props(count);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &count, props.data());

	for (u32 i = 0; const auto& family : props)
	{
		if (family.queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			indices.Graphics = i;
		}

		if (indices.IsComplete())
		{
			break;
		}

		i++;
	}

	return indices;
}

VkPhysicalDevice PickPhysicalDevice()
{
	u32 count;
	VkCall(vkEnumeratePhysicalDevices(s_Instance, &count, nullptr));
	if (!count)
	{
		CRITICAL("Vulkan is installed, but no GPUs support it");
	}
	std::vector<VkPhysicalDevice> devices(count);
	VkCall(vkEnumeratePhysicalDevices(s_Instance, &count, devices.data()));

	u64 lastSuff = -1;
	for (u64 i = 0; const auto& device : devices)
	{
		VkPhysicalDeviceProperties props;
		vkGetPhysicalDeviceProperties(device, &props);
		bool sufficient = GetQueueFamilies(device).IsComplete();
		if (props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && sufficient)
		{
			return device;
		}
		else if (sufficient)
		{
			lastSuff = i;
		}

		i++;
	}

	if (lastSuff != -1)
	{
		return devices[lastSuff];
	}
	else
	{
		CRITICAL("GPUs with Vulkan support exist, but none of them meet Pebble's requirements");
	}
}

std::vector<const char*> GetDeviceExtensions(VkPhysicalDevice device)
{
	u32 count;
	VkCall(vkEnumerateDeviceExtensionProperties(device, nullptr, &count, nullptr));
	std::vector<VkExtensionProperties> properties(count);
	VkCall(vkEnumerateDeviceExtensionProperties(device, nullptr, &count, properties.data()));

	for (const auto& extension : properties)
	{
		if (strcmp(extension.extensionName, "VK_KHR_portability_subset") == 0)
		{
			return { "VK_KHR_portability_subset", VK_KHR_SWAPCHAIN_EXTENSION_NAME };
		}
	}

	return { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
}

void CreateDevice(VkPhysicalDevice phyDevice)
{
	auto families = GetQueueFamilies(phyDevice);

	std::vector<VkDeviceQueueCreateInfo> queues;
	float priority = 1.f;
	for (const auto& family : families)
	{
		VkDeviceQueueCreateInfo info{ .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
			.queueFamilyIndex = family.value(),
			.queueCount = 1,
			.pQueuePriorities = &priority };

		queues.emplace_back(info);
	}

	auto extensions = GetDeviceExtensions(phyDevice);
	auto layers = GetInstanceLayers();

	VkPhysicalDeviceFeatures features{};

	VkDeviceCreateInfo info{
		.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
		.queueCreateInfoCount = u32(queues.size()),
		.pQueueCreateInfos = queues.data(),
		.enabledLayerCount = u32(layers.size()),
		.ppEnabledLayerNames = layers.data(),
		.enabledExtensionCount = u32(extensions.size()),
		.ppEnabledExtensionNames = extensions.data(),
	};

	VkCall(vkCreateDevice(phyDevice, &info, nullptr, &s_Device));

	volkLoadDevice(s_Device);

	vkGetDeviceQueue(s_Device, families.Graphics.value(), 0, &s_GraphicsQueue);
	s_GraphicsQueueIndex = families.Graphics.value();

	VmaVulkanFunctions vkFuncs{ .vkGetPhysicalDeviceProperties = vkGetPhysicalDeviceProperties,
		.vkGetPhysicalDeviceMemoryProperties = vkGetPhysicalDeviceMemoryProperties,
		.vkAllocateMemory = vkAllocateMemory,
		.vkFreeMemory = vkFreeMemory,
		.vkMapMemory = vkMapMemory,
		.vkUnmapMemory = vkUnmapMemory,
		.vkFlushMappedMemoryRanges = vkFlushMappedMemoryRanges,
		.vkInvalidateMappedMemoryRanges = vkInvalidateMappedMemoryRanges,
		.vkBindBufferMemory = vkBindBufferMemory,
		.vkBindImageMemory = vkBindImageMemory,
		.vkGetBufferMemoryRequirements = vkGetBufferMemoryRequirements,
		.vkGetImageMemoryRequirements = vkGetImageMemoryRequirements,
		.vkCreateBuffer = vkCreateBuffer,
		.vkDestroyBuffer = vkDestroyBuffer,
		.vkCreateImage = vkCreateImage,
		.vkDestroyImage = vkDestroyImage,
		.vkCmdCopyBuffer = vkCmdCopyBuffer,
		.vkGetBufferMemoryRequirements2KHR = vkGetBufferMemoryRequirements2KHR,
		.vkGetImageMemoryRequirements2KHR = vkGetImageMemoryRequirements2KHR,
		.vkBindBufferMemory2KHR = vkBindBufferMemory2KHR,
		.vkBindImageMemory2KHR = vkBindImageMemory2KHR,
		.vkGetPhysicalDeviceMemoryProperties2KHR = vkGetPhysicalDeviceMemoryProperties2KHR };

	VmaAllocatorCreateInfo aInfo{
		.physicalDevice = phyDevice, .device = s_Device, .pVulkanFunctions = &vkFuncs, .instance = s_Instance
	};
	VkCall(vmaCreateAllocator(&aInfo, &s_Allocator));

	s_PhysicalDevice = phyDevice;
}

VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT severity,
	VkDebugUtilsMessageTypeFlagsEXT type, const VkDebugUtilsMessengerCallbackDataEXT* callback, void* user)
{
	switch (severity)
	{
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
		TRACE("Vulkan: {}", callback->pMessage);
		break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
		INFO("Vulkan: {}", callback->pMessage);
		break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
		WARN("Vulkan: {}", callback->pMessage);
		break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
		ERROR("Vulkan: {}", callback->pMessage);
		break;
	default:
		break;
	}

	return VK_FALSE;
}

VkInstance Instance() { return s_Instance; }

VkDevice Device() { return s_Device; }

VkPhysicalDevice PhysicalDevice() { return s_PhysicalDevice; }

VmaAllocator Allocator() { return s_Allocator; }

u32 GraphicsIndex() { return s_GraphicsQueueIndex; }

VkQueue GraphicsQueue() { return s_GraphicsQueue; }

void WaitForIdle() { vkDeviceWaitIdle(s_Device); }

void Submit(std::span<CommandBuffer*> buffers, std::span<std::pair<const Semaphore*, VkPipelineStageFlags>> wait,
	std::span<const Semaphore*> signal, const Fence* notify)
{
	// A lot of allocation going on here, so I made it static. Submitting from multiple threads is not allowed, so the
	// vectors don't have to be thread_local.
	static std::vector<VkSemaphore> waitSemaphores;
	static std::vector<VkPipelineStageFlags> waitStages;
	static std::vector<VkSemaphore> signalSemaphores;
	static std::vector<VkCommandBuffer> commandBuffers;

	waitSemaphores.clear();
	waitStages.clear();
	signalSemaphores.clear();
	commandBuffers.clear();

	waitSemaphores.reserve(wait.size());
	waitStages.reserve(wait.size());
	signalSemaphores.reserve(signal.size());
	commandBuffers.reserve(buffers.size());

	for (auto buffer : buffers)
	{
		commandBuffers.push_back(buffer->GetHandle());
	}

	for (auto pair : wait)
	{
		waitSemaphores.push_back(pair.first->GetHandle());
		waitStages.push_back(pair.second);
	}

	for (auto sem : signal)
	{
		signalSemaphores.push_back(sem->GetHandle());
	}

	VkSubmitInfo info{ .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.waitSemaphoreCount = u32(wait.size()),
		.pWaitSemaphores = waitSemaphores.data(),
		.pWaitDstStageMask = waitStages.data(),
		.commandBufferCount = u32(commandBuffers.size()),
		.pCommandBuffers = commandBuffers.data(),
		.signalSemaphoreCount = u32(signalSemaphores.size()),
		.pSignalSemaphores = signalSemaphores.data() };

	VkCall(vkQueueSubmit(s_GraphicsQueue, 1, &info, notify ? notify->GetHandle() : VK_NULL_HANDLE));
}

}

// Thank you Sascha Willems
static const char* ResultToString(VkResult result)
{
	switch (result)
	{
#define STR(r)                                                                                                         \
	case VK_##r:                                                                                                       \
		return #r
		STR(NOT_READY);
		STR(TIMEOUT);
		STR(EVENT_SET);
		STR(EVENT_RESET);
		STR(INCOMPLETE);
		STR(ERROR_OUT_OF_HOST_MEMORY);
		STR(ERROR_OUT_OF_DEVICE_MEMORY);
		STR(ERROR_INITIALIZATION_FAILED);
		STR(ERROR_DEVICE_LOST);
		STR(ERROR_MEMORY_MAP_FAILED);
		STR(ERROR_LAYER_NOT_PRESENT);
		STR(ERROR_EXTENSION_NOT_PRESENT);
		STR(ERROR_FEATURE_NOT_PRESENT);
		STR(ERROR_INCOMPATIBLE_DRIVER);
		STR(ERROR_TOO_MANY_OBJECTS);
		STR(ERROR_FORMAT_NOT_SUPPORTED);
		STR(ERROR_SURFACE_LOST_KHR);
		STR(ERROR_NATIVE_WINDOW_IN_USE_KHR);
		STR(SUBOPTIMAL_KHR);
		STR(ERROR_OUT_OF_DATE_KHR);
		STR(ERROR_INCOMPATIBLE_DISPLAY_KHR);
		STR(ERROR_VALIDATION_FAILED_EXT);
		STR(ERROR_INVALID_SHADER_NV);
#undef STR
	default:
		return "UNKNOWN_ERROR";
	}
}

void HandleVkResult(VkResult result, const char* statement, const char* file, int line)
{
	if (result != VK_SUCCESS)
	{
		CRITICAL("VkResult '{}' at '{}', in file '{}', at line {}", ResultToString(result), statement, file, line);
	}
}
