#pragma once

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stdexcept>
#include <memory>

#include "Camera.h"

const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 800;

class MVWindow
{
public:

	MVWindow();

	GLFWwindow* windowPtr() { return window; }
	void processInput(GLFWwindow* window, float deltaTime);
	// this is public so that the shader uniforms in main have access, may need to write a getter and make it private?
	Camera camera{ glm::vec3(0.f, 0.f, 3.f) };

private:

	
	GLFWwindow* window;
	double lastX = SCR_WIDTH / 2.f;
	double lastY = SCR_HEIGHT / 2.f;
	bool firstMouse = true;
	bool right_mouse_button_pushed{ false };

	void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
	void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
};

