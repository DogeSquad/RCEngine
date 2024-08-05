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
		void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);
		VkExtent2D getExtent();
	private:
		
		void initWindow();

		const int width;
		const int height;

		std::string windowName;
		GLFWwindow* window;
	};
}