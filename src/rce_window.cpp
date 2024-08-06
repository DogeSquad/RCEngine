#include "rce_window.hpp"

#include <stdexcept>

namespace rce 
{
RCEWindow::RCEWindow(int w, int h, std::string name) :
	width{ w },
	height{ h },
	windowName{ name }
{
	initWindow();
}
RCEWindow::~RCEWindow()
{
	glfwDestroyWindow(window);
	glfwTerminate();
}

bool RCEWindow::shouldClose()
{
	return glfwWindowShouldClose(window);
}

void RCEWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface)
{
	if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS)
		throw std::runtime_error("failed to create window surface");
}

VkExtent2D RCEWindow::getExtent()
{
	return { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };
}

void RCEWindow::framebufferResizeCallback(GLFWwindow* window, int width, int height)
{
	auto rceWindow = reinterpret_cast<RCEWindow*>(glfwGetWindowUserPointer(window));
	rceWindow->framebufferResized = true;
	rceWindow->width = width;
	rceWindow->height = height;
}

void RCEWindow::initWindow()
{
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

	window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
	glfwSetWindowUserPointer(window, this);
	glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
}

}