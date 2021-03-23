#pragma once

#include "Instance.h"
#include "Image.h"

class Semaphore;
class Fence;

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

	VkFormat GetFormat() const { return m_Format; }
	const std::vector<ImageView>& GetViews() const { return m_Views; }
	glm::u32vec2 GetSize() const { return m_Size; }

	std::optional<u32> GetNextImage(const Semaphore* semaphore, const Fence* fence, u64 timeout = -1) const;
	static void Present(std::span<const Swapchain*> swapchains, std::span<const Semaphore*> wait, std::span<u32> indices);

private:
	static void FramebufferResizeCallback(GLFWwindow* window, int width, int height);

	void Recreate();

	GLFWwindow* m_Window = nullptr;
	VkSurfaceKHR m_Surface = VK_NULL_HANDLE;
	VkSwapchainKHR m_Swapchain = VK_NULL_HANDLE;

	VkFormat m_Format;

	std::vector<VkImage> m_Images;
	std::vector<ImageView> m_Views;
	bool m_Stalled = false;

	glm::u32vec2 m_Size;
	std::function<void(u32, u32)> m_PreResizeCallback;
	std::function<void(u32, u32)> m_PostResizeCallback;
};
