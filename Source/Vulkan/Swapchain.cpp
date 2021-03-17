#include "PCH.h"

#include "Swapchain.h"

#include "Window/Window.h"

Swapchain::Swapchain(GLFWwindow* target)
{
	VkCall(glfwCreateWindowSurface(Instance::Instance(), target, nullptr, &m_Surface));
	glfwSetFramebufferSizeCallback(target, &FramebufferResizeCallback);

	Recreate();

	TRACE("Created swapchain");
}

void Swapchain::SetPreResizeCallback(std::function<void(u32, u32)> callback) { m_PreResizeCallback = callback; }

void Swapchain::SetPostResizeCallback(std::function<void(u32, u32)> callback) { m_PostResizeCallback = callback; }

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
}

Swapchain& Swapchain::operator=(Swapchain&& other)
{
	this->~Swapchain();

	m_Surface = other.m_Surface;
	other.m_Surface = VK_NULL_HANDLE;
	m_Swapchain = other.m_Swapchain;
	other.m_Swapchain = VK_NULL_HANDLE;
	m_Window = other.m_Window;

	return *this;
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
		extent.width = std::max(support.Capabilities.minImageExtent.width,
			std::min(support.Capabilities.maxImageExtent.width, uint32_t(width)));
		extent.height = std::max(support.Capabilities.minImageExtent.height,
			std::min(support.Capabilities.maxImageExtent.height, uint32_t(height)));

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
