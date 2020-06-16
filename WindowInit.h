#pragma once

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stdexcept>
#include <memory>

#include "Camera.h"

class WindowInit
{
public:

	WindowInit();

	GLFWwindow* windowPtr() { return window.get(); }
	void processInput(GLFWwindow* window, float deltaTime);
	// camera, this is global so that the keyboard control callbacks have access
	Camera camera{ glm::vec3(0.f, 0.f, 3.f) };

private:

	std::unique_ptr<GLFWwindow> window;

	const unsigned int SCR_WIDTH = 1200;
	const unsigned int SCR_HEIGHT = 800;

	double lastX = SCR_WIDTH / 2.f;
	double lastY = SCR_HEIGHT / 2.f;
	bool firstMouse = true;
	bool right_mouse_button_pushed{ false };

	void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
	void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
};

