
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

const unsigned int SCR_WIDTH = 600;
const unsigned int SCR_HEIGHT = 800;

// camera, this is global so that the keyboard control callbacks have access
Camera camera(glm::vec3(0.f, 0.f, 3.f));
double lastX = SCR_WIDTH / 2.f;
double lastY = SCR_HEIGHT / 2.f;
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
	
	double xoffset = xpos - lastX;
	double yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(static_cast<float>(xoffset), static_cast<float>(yoffset));
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(static_cast<float>(yoffset));
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

	// light positions
	 // positions of the point lights
	std::array<glm::vec3, 2> pointLightPositions = {
		glm::vec3(2.3f, -3.3f, -4.0f),
		glm::vec3(0.7f,  0.2f,  2.0f)
	};

	Shader objectShader("shaders/model.vert", "shaders/model.frag");
	//Shader lightingShader("shaders/shader.vert", "shaders/multiple_light_shader.frag");
	Shader textShader("shaders/text_shader.vert", "shaders/text_shader.frag");
	// enable depth testing
	glEnable(GL_DEPTH_TEST);
	

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
		static double startTime = glfwGetTime();
		double elapsedTime = glfwGetTime() - startTime;
		fps = static_cast<int>(countedFrames / elapsedTime);
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);

		// render commands
		glClearColor(0.f, 0.f, 0.f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// don't forget to enable shader before setting uniforms
		objectShader.use();

		// set up lighting uniforms
		objectShader.setVec3("viewPos", camera.Position);
		objectShader.setFloat("shininess", 32.f);
		// point light 1
		objectShader.setVec3("pointLights[0].position", pointLightPositions[0]);
		objectShader.setVec3("pointLights[0].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
		objectShader.setVec3("pointLights[0].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
		objectShader.setVec3("pointLights[0].specular", glm::vec3(1.0f, 1.0f, 1.0f));
		objectShader.setFloat("pointLights[0].constant", 1.0f);
		objectShader.setFloat("pointLights[0].linear", 0.09f);
		objectShader.setFloat("pointLights[0].quadratic", 0.012f);
		// point light 2
		objectShader.setVec3("pointLights[1].position", pointLightPositions[1]);
		objectShader.setVec3("pointLights[1].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
		objectShader.setVec3("pointLights[1].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
		objectShader.setVec3("pointLights[1].specular", glm::vec3(1.0f, 1.0f, 1.0f));
		objectShader.setFloat("pointLights[1].constant", 1.0f);
		objectShader.setFloat("pointLights[1].linear", 0.09f);
		objectShader.setFloat("pointLights[1].quadratic", 0.032f);


		
		// view/projection transformations
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		objectShader.setMat4("projection", projection);
		objectShader.setMat4("view", view);

		// render the loaded model
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f)); // translate it down so it's at the center of the scene
		model = glm::scale(model, glm::vec3(0.15f));	// it's a bit too big for our scene, so scale it down
		objectShader.setMat4("model", model);
		ourModel.Draw(objectShader);

		// enable blending for text rendering
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		// activate text shader and render text
		textShader.use();
		glm::mat4 textProjection = glm::ortho(0.f, 800.f, 0.f, 600.f);
		textShader.setMat4("textProjection", textProjection);
		textObject.RenderText(textShader, "FPS: " + std::to_string(fps), 25.f, 25.f, 0.25f, glm::vec3(0.5f, 0.8f, 0.2f), textVAO, textVBO);
		textObject.RenderText(textShader, "Camera xpos: " + std::to_string(camera.Position.x) + 
			 " ypos: " + std::to_string(camera.Position.y), 25.f, 50.f, 0.25f, glm::vec3(1.f, 0.f, 0.f), textVAO, textVBO);
		glDisable(GL_BLEND);
		//____________________________
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
}