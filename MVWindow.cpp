#include "MVWindow.h"


MVWindow::MVWindow()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	const char* glsl_version = "#version 330";

	window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Model Viewer", nullptr, nullptr);

	if (window == nullptr)
	{
		// std::cout << "failed to create GLFW window" << std::endl;
		glfwTerminate();
		throw std::runtime_error("failed to create GLFW window");
	}
	glfwMakeContextCurrent(window);

	// set up casting for callbacks
	MVWindow* myWindow = this;
	glfwSetWindowUserPointer(window, myWindow);

	auto frameBufferCallBack = [](GLFWwindow* w, int width, int height)
	{
		static_cast<MVWindow*>(glfwGetWindowUserPointer(w))->framebuffer_size_callback(w, width, height);
	};
	glfwSetFramebufferSizeCallback(window, frameBufferCallBack);

	auto mouseButtonCallBack = [](GLFWwindow * w, int button, int action, int mods)
	{
		static_cast<MVWindow*>(glfwGetWindowUserPointer(w))->mouse_button_callback(w, button, action, mods);
	};
	glfwSetMouseButtonCallback(window, mouseButtonCallBack);

	auto mouseCallBack = [](GLFWwindow* w, double xpos, double ypos)
	{
		static_cast<MVWindow*>(glfwGetWindowUserPointer(w))-> mouse_callback(w, xpos, ypos);
	};
	glfwSetCursorPosCallback(window, mouseCallBack);

	auto scrollCallBack = [](GLFWwindow* w, double xoffset, double yoffset)
	{
		static_cast<MVWindow*>(glfwGetWindowUserPointer(w))->scroll_callback(w, xoffset, yoffset);
	};
	glfwSetScrollCallback(window, scrollCallBack);
	

	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


	// GLADS setup
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		// std::cout << "failed to initialize GLAD" << std::endl;
		throw std::runtime_error("failed to initialize GLAD");
	}

	// IMGUI setup
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);
}

void MVWindow::processInput(GLFWwindow* window, float deltaTime)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}

	// set wireframe
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	// set fill
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	// camera movement
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) { camera.ProcessKeyboard(Camera_Movement::FORWARD, deltaTime); }
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) { camera.ProcessKeyboard(Camera_Movement::LEFT, deltaTime); }
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) { camera.ProcessKeyboard(Camera_Movement::BACKWARD, deltaTime); }
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) { camera.ProcessKeyboard(Camera_Movement::RIGHT, deltaTime); }
}

void MVWindow::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void MVWindow::mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
		right_mouse_button_pushed = true;
	else
		right_mouse_button_pushed = false;
}

void MVWindow::mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	double xoffset = xpos - lastX;
	double yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	if (right_mouse_button_pushed)
	{
		camera.ProcessMouseMovement(static_cast<float>(xoffset), static_cast<float>(yoffset));
	}
}

void MVWindow::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(static_cast<float>(yoffset));
}
