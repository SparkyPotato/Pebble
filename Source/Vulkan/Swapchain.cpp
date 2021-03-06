#include "PCH.h"

#include "Swapchain.h"

#include "Window/Window.h"

#include "Sync.h"

Swapchain::Swapchain(GLFWwindow* target) : m_Window(target)
{
	VkCall(glfwCreateWindowSurface(Instance::Instance(), target, nullptr, &m_Surface));
	glfwSetFramebufferSizeCallback(target, &FramebufferResizeCallback);

	Recreate();

	TRACE("Created swapchain");
}

Swapchain::~Swapchain()
{
	vkDestroySwapchainKHR(Instance::Device(), m_Swapchain, nullptr);
	vkDestroySurfaceKHR(Instance::Instance(), m_Surface, nullptr);
}

Swapchain::Swapchain(Swapchain&& other)
{
	m_Surface = other.m_Surface;
	other.m_Surface = VK_NULL_HANDLE;
	m_Swapchain = other.m_Swapchain;
	other.m_Swapchain = VK_NULL_HANDLE;

	m_Window = other.m_Window;

	m_Format = other.m_Format;
	m_Stalled = other.m_Stalled;
	m_Size = other.m_Size;

	m_Images = std::move(other.m_Images);
	m_Views = std::move(other.m_Views);
	m_PreResizeCallback = std::move(other.m_PreResizeCallback);
	m_PostResizeCallback = std::move(other.m_PostResizeCallback);
}

Swapchain& Swapchain::operator=(Swapchain&& other)
{
	this->~Swapchain();

	m_Surface = other.m_Surface;
	other.m_Surface = VK_NULL_HANDLE;
	m_Swapchain = other.m_Swapchain;
	other.m_Swapchain = VK_NULL_HANDLE;

	m_Window = other.m_Window;

	m_Format = other.m_Format;
	m_Stalled = other.m_Stalled;
	m_Size = other.m_Size;

	m_Images = std::move(other.m_Images);
	m_Views = std::move(other.m_Views);
	m_PreResizeCallback = std::move(other.m_PreResizeCallback);
	m_PostResizeCallback = std::move(other.m_PostResizeCallback);

	return *this;
}

void Swapchain::SetPreResizeCallback(std::function<void(u32, u32)> callback) { m_PreResizeCallback = callback; }

void Swapchain::SetPostResizeCallback(std::function<void(u32, u32)> callback) { m_PostResizeCallback = callback; }

std::optional<u32> Swapchain::GetNextImage(const Semaphore* semaphore, const Fence* fence, u64 timeout) const
{
	if (m_Stalled)
	{
		return std::nullopt;
	}

	u32 ret = -1;
	VkResult res = vkAcquireNextImageKHR(Instance::Device(), m_Swapchain, timeout,
		semaphore ? semaphore->GetHandle() : VK_NULL_HANDLE, fence ? fence->GetHandle() : VK_NULL_HANDLE, &ret);

	if (res != VK_SUCCESS || ret == -1)
	{
		if (res != VK_ERROR_OUT_OF_DATE_KHR && res != VK_SUBOPTIMAL_KHR)
		{
			CRITICAL("Failed to acquire swapchain image");
		}

		return std::nullopt;
	}

	return ret;
}

void Swapchain::Present(
	std::span<const Swapchain*> swapchains, std::span<const Semaphore*> wait, std::span<u32> indices)
{
	static std::vector<VkSemaphore> waitSemaphores;
	static std::vector<VkSwapchainKHR> vkSwapchains;
	static std::vector<VkResult> results;

	waitSemaphores.clear();
	vkSwapchains.clear();

	waitSemaphores.reserve(wait.size());
	vkSwapchains.reserve(swapchains.size());
	results.resize(swapchains.size());

	for (auto sem : wait)
	{
		waitSemaphores.push_back(sem->GetHandle());
	}
	for (auto swapchain : swapchains)
	{
		if (!swapchain->m_Stalled)
		{
			vkSwapchains.push_back(swapchain->m_Swapchain);
		}
	}

	VkPresentInfoKHR info{ .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
		.waitSemaphoreCount = u32(waitSemaphores.size()),
		.pWaitSemaphores = waitSemaphores.data(),
		.swapchainCount = u32(vkSwapchains.size()),
		.pSwapchains = vkSwapchains.data(),
		.pImageIndices = indices.data(),
		.pResults = results.data() };

	vkQueuePresentKHR(Instance::GraphicsQueue(), &info);

	for (auto res : results)
	{
		if (res != VK_SUCCESS && res != VK_ERROR_OUT_OF_DATE_KHR && res != VK_SUBOPTIMAL_KHR)
		{
			CRITICAL("Failed to present");
		}
	}
}

struct Support
{
	VkSurfaceCapabilitiesKHR Capabilities;
	std::vector<VkSurfaceFormatKHR> Formats;
	std::vector<VkPresentModeKHR> PresentModes;
};

Support GetSurfaceSupport(VkSurfaceKHR surface)
{
	Support support{};

	VkCall(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(Instance::PhysicalDevice(), surface, &support.Capabilities));

	u32 count;
	VkCall(vkGetPhysicalDeviceSurfaceFormatsKHR(Instance::PhysicalDevice(), surface, &count, nullptr));
	support.Formats.resize(count);
	VkCall(vkGetPhysicalDeviceSurfaceFormatsKHR(Instance::PhysicalDevice(), surface, &count, support.Formats.data()));

	VkCall(vkGetPhysicalDeviceSurfacePresentModesKHR(Instance::PhysicalDevice(), surface, &count, nullptr));
	support.PresentModes.resize(count);
	VkCall(vkGetPhysicalDeviceSurfacePresentModesKHR(
		Instance::PhysicalDevice(), surface, &count, support.PresentModes.data()));

	return support;
}

struct Options
{
	VkExtent2D Size;
	VkSurfaceFormatKHR Format;
	VkPresentModeKHR PresentMode;
	u32 ImageCount;
};

VkSurfaceFormatKHR GetFormat(const Support& support)
{
	for (auto& format : support.Formats)
	{
		if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
		{
			return format;
		}
	}

	return support.Formats[0];
}

VkPresentModeKHR GetPresentMode(const Support& support)
{
	for (auto& mode : support.PresentModes)
	{
		if (mode == VK_PRESENT_MODE_MAILBOX_KHR)
		{
			return mode;
		}
	}

	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D GetExtent(GLFWwindow* window, const Support& support)
{
	if (support.Capabilities.currentExtent.width != -1)
	{
		return support.Capabilities.currentExtent;
	}
	else
	{
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);

		VkExtent2D extent;
		extent.width = std::clamp(
			u32(width), support.Capabilities.minImageExtent.width, support.Capabilities.maxImageExtent.width);
		extent.height = std::clamp(
			u32(height), support.Capabilities.minImageExtent.height, support.Capabilities.maxImageExtent.height);

		return extent;
	}
}

Options GetSwapchainOptions(GLFWwindow* window, const Support& support)
{
	if (support.Formats.empty() || support.PresentModes.empty())
	{
		CRITICAL("Surface doesn't support any formats or present modes");
	}

	Options options{};
	options.Format = GetFormat(support);
	options.PresentMode = GetPresentMode(support);
	options.Size = GetExtent(window, support);

	options.ImageCount = support.Capabilities.minImageCount + 2;
	if (support.Capabilities.maxImageCount != 0 && options.ImageCount > support.Capabilities.maxImageCount)
	{
		options.ImageCount = support.Capabilities.maxImageCount;
	}

	return options;
}

void Swapchain::Recreate()
{
	auto support = GetSurfaceSupport(m_Surface);
	auto options = GetSwapchainOptions(m_Window, support);
	auto oldSwapchain = m_Swapchain;

	m_Format = options.Format.format;
	m_Size = { options.Size.width, options.Size.height };

	VkSwapchainCreateInfoKHR info{ .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
		.surface = m_Surface,
		.minImageCount = options.ImageCount,
		.imageFormat = options.Format.format,
		.imageColorSpace = options.Format.colorSpace,
		.imageExtent = options.Size,
		.imageArrayLayers = 1,
		.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
		.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
		.preTransform = support.Capabilities.currentTransform,
		.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
		.presentMode = options.PresentMode,
		.clipped = VK_TRUE,
		.oldSwapchain = oldSwapchain };

	VkBool32 present = false;
	VkCall(vkGetPhysicalDeviceSurfaceSupportKHR(
		Instance::PhysicalDevice(), Instance::GraphicsIndex(), m_Surface, &present));
	if (!present)
	{
		CRITICAL("Cannot present to window surface");
	}

	VkCall(vkCreateSwapchainKHR(Instance::Device(), &info, nullptr, &m_Swapchain));
	vkDestroySwapchainKHR(Instance::Device(), oldSwapchain, nullptr);

	u32 count;
	VkCall(vkGetSwapchainImagesKHR(Instance::Device(), m_Swapchain, &count, nullptr));
	m_Images.resize(count);
	VkCall(vkGetSwapchainImagesKHR(Instance::Device(), m_Swapchain, &count, m_Images.data()));
	m_Views.resize(count);

	for (u64 i = 0; auto image : m_Images)
	{
		m_Views[i] = ImageView(image, options.Format.format, VK_IMAGE_VIEW_TYPE_2D,
			VkComponentMapping{ .r = VK_COMPONENT_SWIZZLE_IDENTITY,
				.g = VK_COMPONENT_SWIZZLE_IDENTITY,
				.b = VK_COMPONENT_SWIZZLE_IDENTITY,
				.a = VK_COMPONENT_SWIZZLE_IDENTITY },
			VkImageSubresourceRange{ .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
				.baseMipLevel = 0,
				.levelCount = 1,
				.baseArrayLayer = 0,
				.layerCount = 1 });

		i++;
	}
}

void Swapchain::FramebufferResizeCallback(GLFWwindow* window, int width, int height)
{
	auto& swapchain = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window))->m_Swapchain;
	swapchain.m_Stalled = width == 0 || height == 0;

	if (!swapchain.m_Stalled)
	{
		if (swapchain.m_PreResizeCallback)
		{
			swapchain.m_PreResizeCallback(width, height);
		}

		swapchain.Recreate();

		if (swapchain.m_PostResizeCallback)
		{
			swapchain.m_PostResizeCallback(width, height);
		}
	}
}
