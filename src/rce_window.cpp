#include "rce_window.hpp"

#include <stdexcept>

rce::RCEWindow::RCEWindow(int w, int h, std::string name) :
	width{ w },
	height{ h },
	windowName{ name } 
{
	initWindow();
}
rce::RCEWindow::~RCEWindow() 
{
	glfwDestroyWindow(window);
	glfwTerminate();
}

bool rce::RCEWindow::shouldClose() 
{
	return glfwWindowShouldClose(window);
}

void rce::RCEWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface)
{
	if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS)
		throw std::runtime_error("failed to create window surface");
}

VkExtent2D rce::RCEWindow::getExtent()
{
	return { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };
}

void rce::RCEWindow::initWindow() 
{
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
}
