#pragma once

#include <string>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace rce {
	class RCEWindow {
	public:
		RCEWindow(int w, int h, std::string name);
		~RCEWindow();

		RCEWindow(const RCEWindow&) = delete;
		RCEWindow& operator=(const RCEWindow&) = delete;

		bool shouldClose();
		VkExtent2D getExtent();
		bool wasWindowResized() { return framebufferResized; }
		void resetWindowResizedFlag() { framebufferResized = false; }

		void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);
	private:
		static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
		void initWindow();

		int width;
		int height;
		bool framebufferResized = false; 

		std::string windowName;
		GLFWwindow* window;
	};
}