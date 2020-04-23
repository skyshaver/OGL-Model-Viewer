
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <ft2build.h>
#include FT_FREETYPE_H

// do this in a cpp never in a header
#define STB_IMAGE_IMPLEMENTATION

#include <iostream>
#include <map>
#include <utility>
#include <array>

#include "Camera.h"
#include "TextObject.h"
#include "Model.h"

const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 1000;

// camera, this is global so that the keyboard control callbacks have access
Camera camera(glm::vec3(0.f, 0.f, 3.f));
float lastX = SCR_WIDTH / 2.f;
float lastY = SCR_HEIGHT / 2.f;
bool firstMouse = true;
// time delta
float deltaTime = 0.f;
float lastFrame = 0.f;
// light box pos
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}
	
	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

void processInput(GLFWwindow* window) 
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


int main() 
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	const char* glsl_version = "#version 330";

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOGL", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "failed to initialize GLAD" << std::endl;
		return -1;
	}

	TextObject textObject("fonts/arial.ttf");


	Shader lightingShader("shaders/model.vert", "shaders/model.frag");
	//Shader lampShader("shaders/light_shader.vert", "shaders/light_shader.frag");
	Shader textShader("shaders/text_shader.vert", "shaders/text_shader.frag");
	// enable depth testing
	glEnable(GL_DEPTH_TEST);
	// enable blending for text rendering
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// buffer for text
	GLuint textVAO, textVBO;
	glGenVertexArrays(1, &textVAO);
	glGenBuffers(1, &textVBO);
	glBindVertexArray(textVAO);
	glBindBuffer(GL_ARRAY_BUFFER, textVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);


	Model ourModel("models/nanosuit/nanosuit.obj");

	int countedFrames = 0;
	int fps = 0;
	while (!glfwWindowShouldClose(window))
	{
		countedFrames++;
		static float startTime = glfwGetTime();
		float elapsedTime = glfwGetTime() - startTime;
		fps = countedFrames / elapsedTime;
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);

		// render commands
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// don't forget to enable shader before setting uniforms
		lightingShader.use();

		// view/projection transformations
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		lightingShader.setMat4("projection", projection);
		lightingShader.setMat4("view", view);

		// render the loaded model
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f)); // translate it down so it's at the center of the scene
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));	// it's a bit too big for our scene, so scale it down
		lightingShader.setMat4("model", model);
		ourModel.Draw(lightingShader);

		// activate text shader and render text
		textShader.use();
		glm::mat4 textProjection = glm::ortho(0.f, 800.f, 0.f, 600.f);
		textShader.setMat4("textProjection", textProjection);
		textObject.RenderText(textShader, "FPS: " + std::to_string(fps), 25.f, 25.f, 0.25f, glm::vec3(0.5f, 0.8f, 0.2f), textVAO, textVBO);
		textObject.RenderText(textShader, "Camera xpos: " + std::to_string(camera.Position.x) + 
			 " ypos: " + std::to_string(camera.Position.y), 25.f, 50.f, 0.25f, glm::vec3(1.f, 0.f, 0.f), textVAO, textVBO);

		//____________________________
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
}