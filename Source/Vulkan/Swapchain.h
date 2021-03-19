#pragma once

#include "Instance.h"
#include "Image.h"

class Swapchain
{
public:
	Swapchain() = default;
	Swapchain(GLFWwindow* target);
	~Swapchain();

	Swapchain(const Swapchain& other) = delete;
	Swapchain& operator=(const Swapchain& other) = delete;

	Swapchain(Swapchain&& other);
	Swapchain& operator=(Swapchain&& other);

	void SetPreResizeCallback(std::function<void(u32, u32)> callback);
	void SetPostResizeCallback(std::function<void(u32, u32)> callback);

	bool IsStalled() { return m_Stalled; }

private:
	static void FramebufferResizeCallback(GLFWwindow* window, int width, int height);

	void Recreate();

	GLFWwindow* m_Window = nullptr;
	VkSurfaceKHR m_Surface = VK_NULL_HANDLE;
	VkSwapchainKHR m_Swapchain = VK_NULL_HANDLE;
	std::vector<VkImage> m_Images;
	std::vector<ImageView> m_Views;
	bool m_Stalled = false;

	std::function<void(u32, u32)> m_PreResizeCallback;
	std::function<void(u32, u32)> m_PostResizeCallback;
};
