#pragma once



#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stdexcept>
#include <string>

#include "Camera.h"

class MVWindow
{
public:

	MVWindow(const unsigned int SCR_WIDTH, const unsigned int SCR_HEIGHT, const std::string& name);

	~MVWindow() { glfwDestroyWindow(window); }

	GLFWwindow* windowPtr() const { return window; }
	const unsigned int getHeight() const { return SCR_HEIGHT; }
	const unsigned int getWidth() const { return SCR_WIDTH; }
	const char* glslVersion() const { return glsl_version; }
	void processInput(GLFWwindow* window, float deltaTime);
	// this is public so that the shader uniforms in main have access, TODO may need to write a getter and make it private?
	Camera camera{ glm::vec3(0.f, 0.f, 3.f) };

private:

	GLFWwindow* window;

	const unsigned int SCR_WIDTH; 
	const unsigned int SCR_HEIGHT;
	const char* glsl_version = "#version 330";
	double lastX = SCR_WIDTH / 2.f;
	double lastY = SCR_HEIGHT / 2.f;
	bool firstMouse = true;
	bool right_mouse_button_pushed = false;

	void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
	void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
};

