
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>


#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <iostream>
#include <map>
#include <utility>
#include <string>

#include "Shader.h"
#include "Camera.h"
#include "TextObject.h"

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

// utility function for loading a 2D texture from file
unsigned int loadTexture(char const* path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

void printMaxVertexAttribs() 
{
	int nrAttributes;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
	std::cout << "Max number of vertex attribs: " << nrAttributes << std::endl;
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

	// cube vertices, not texture data
	//float vertices[] = {
	//-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	// 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	// 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	// 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	//-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	//-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

	//-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	// 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	// 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	// 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	//-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	//-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

	//-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	//-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	//-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	//-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	//-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	//-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

	// 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
	// 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	// 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	// 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	// 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
	// 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

	//-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
	// 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
	// 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	// 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	//-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	//-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

	//-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
	// 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
	// 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	// 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	//-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	//-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
	//};

	float vertices[] = {
		// positions          // normals           // texture coords
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
	};

	Shader lightingShader("shaders/shader.vert", "shaders/shader.frag");
	Shader lampShader("shaders/light_shader.vert", "shaders/light_shader.frag");
	Shader textShader("shaders/text_shader.vert", "shaders/text_shader.frag");
	// enable depth testing
	glEnable(GL_DEPTH_TEST);
	// enable blending for text rendering
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// buffer for boxes
	unsigned int VBO, cubeVAO;// vertex buffer object, vertex arrray object, element buffer object
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindVertexArray(cubeVAO);
	// pos attrib
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// normal attrib
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// texture attrib
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	// buffer for lighting object
	unsigned int lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);
	// we only need to bind to the VBO (to link it with glVertexAttribPointer), no need to fill it; 
	// the VBO's data already contains all we need (it's already bound, but we do it again for educational purposes)
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

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


	// load texture for light maps
	unsigned int diffuseMap = loadTexture("textures/container2.png");
	unsigned int specularMap = loadTexture("textures/container2_specular.png");
	// set up shader for texture map
	lightingShader.use();
	lightingShader.setInt("material.diffuse", 0);
	lightingShader.setInt("material.specular", 1);

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

		// activate lighting shaders
		lightingShader.use();
		
		lightingShader.setVec3("viewPos", camera.Position);

		// uuniforms for lighting material

		// rotating the light
		/*float xValue = 2.0f * (sin(currentFrame));
		float zValue = 1.5f * (cos(currentFrame));
		lightPos = glm::vec3(xValue, 1.f, zValue);*/
		lightingShader.setVec3("light.pos", lightPos);

		// this is for influencing the other light colors
		/*glm::vec3 lightColor;
		lightColor.x = sin(glfwGetTime() * 2.0f);
		lightColor.y = sin(glfwGetTime() * 0.7f);
		lightColor.z = sin(glfwGetTime() * 1.3f);*/

		glm::vec3 ambientColor = glm::vec3(0.2);
		glm::vec3 diffuseColor = glm::vec3(0.5);
		glm::vec3 specularColor = glm::vec3(1.0f);
		lightingShader.setVec3("light.ambientColor", ambientColor);
		lightingShader.setVec3("light.diffuseColor", diffuseColor);
		lightingShader.setVec3("light.specularColor", specularColor);


		// uniforms for material
		glm::vec3 ambient(1.0f, 0.5f, 0.31f);
		glm::vec3 diffuse(1.0f, 0.5f, 0.31f);
		glm::vec3 specular(0.5f, 0.5f, 0.5f);
		// lightingShader.setVec3("material.ambient", ambient);
		// lightingShader.setVec3("material.diffuse", diffuse);
		lightingShader.setVec3("material.specular", specular);
		lightingShader.setFloat("material.shininess", 32.0f);

		// bind diffuse map
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap);
		// bind specular map
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularMap);
		
		// projection and view
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom * 2), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.f);	
		glm::mat4 view = camera.GetViewMatrix();
		lightingShader.setMat4("projection", projection);
		lightingShader.setMat4("view", view);	
		// world transformation
		glm::mat4 model = glm::mat4(1.0f);
		lightingShader.setMat4("model", model);
		// render cube
		glBindVertexArray(cubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		
		// activate lamp shader
		lampShader.use();
		lampShader.setMat4("projection", projection);
		lampShader.setMat4("view", view);
		model = glm::mat4(1.0f);
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
		lampShader.setMat4("model", model);
		// render lamp object
		glBindVertexArray(lightVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

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