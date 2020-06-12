#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"


#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <ft2build.h>
#include FT_FREETYPE_H

// do this in one cpp never in a header
#define STB_IMAGE_IMPLEMENTATION

#include <fmt/format.h>

#include <iostream>
#include <map>
#include <utility>
#include <array>
#include <thread>
#include <chrono>
using namespace std::chrono_literals;

#include "Camera.h"
#include "TextObject.h"
#include "Model.h"
#include "PointLight.h"
#include "color_map.h"

const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 800;

// camera, this is global so that the keyboard control callbacks have access
Camera camera(glm::vec3(0.f, 0.f, 3.f));
double lastX = SCR_WIDTH / 2.f;
double lastY = SCR_HEIGHT / 2.f;
bool firstMouse = true;
// time delta
float deltaTime = 0.f;
float lastFrame = 0.f;
static bool right_mouse_button_pushed = false;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
		right_mouse_button_pushed = true;
	else
		right_mouse_button_pushed = false;
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

	// turned off till I figure out how to separate mouse camera movement from imgui input
	if (right_mouse_button_pushed)
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
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "failed to initialize GLAD" << std::endl;
		return -1;
	}

	// IMGUI setup

	// Setup Dear ImGui context
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

	// IMGUI state
	bool show_demo_window = true;
	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.0f, 0.0f, 0.0f, 1.00f);

	TextObject textObject("fonts/arial.ttf");

	 // positions of the point lights
	std::vector<glm::vec3> pointLightPositions = {
		// x , y, z
		glm::vec3(-3.f,  1.f, 0.0f),
		glm::vec3(3.f,  1.f,  0.0f)
	};
	
	// positions of the point lights, in vec format so they can passed to IMGUI, may be a better way to do this
	std::vector<std::array<float, 3>> pointLightPositionsIG = {
		// x , y, z
		{-3.f,  1.f, 0.0f},
		{3.f,  1.f,  0.0f}
	};

	Shader objectShader("shaders/model.vert", "shaders/model.frag");
	Shader lightShader("shaders/light_shader.vert", "shaders/light_shader.frag");
	Shader textShader("shaders/text_shader.vert", "shaders/text_shader.frag");

	Model ourModel("models/nanosuit/nanosuit.obj");
	Model sphereModel("models/sphere/sphere.obj");

	PointLight plOne;
	PointLight plTwo;

	int countedFrames = 0;
	int fps = 0;

	glEnable(GL_DEPTH_TEST);

	while (!glfwWindowShouldClose(window))
	{
		// frame counter
		countedFrames++;
		static double startTime = glfwGetTime();
		double elapsedTime = glfwGetTime() - startTime;
		fps = static_cast<int>(countedFrames / elapsedTime);

		// calculate frame delta
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// constrain frame rate
		static const double frameDelay = 0.017;
		if (deltaTime < frameDelay)
		{
			auto sleepTime = std::chrono::duration<double>(frameDelay - deltaTime);
			std::this_thread::sleep_for(sleepTime);
		}

		processInput(window);

		// render commands
		glClearColor(clear_color.x, clear_color.y, clear_color.z, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// IMGUI frame
		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// IMGUI window
		// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
		
		static float lightOneAmbient = 0.25f;
		static float lightOneDiffuse = 0.8f;
		static float lightOneSpecular = 1.0f;
		static float lightTwoAmbient = 0.25f;
		static float lightTwoDiffuse = 0.8f;
		static float lightTwoSpecular = 1.0f;
		//static int counter = 0;

		ImGui::Begin("Model Viewer");                          // Create a window called "Hello, world!" and append into it.

		//ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
		//ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
		//ImGui::Checkbox("Another Window", &show_another_window);

		ImGui::SliderFloat("Light 1 Ambient", &lightOneAmbient, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
		ImGui::SliderFloat("Light 1 Diffuse", &lightOneDiffuse, 0.0f, 1.0f);            
		ImGui::SliderFloat("Light 1 Specular", &lightOneSpecular, 0.0f, 1.0f);    
		ImGui::SliderFloat("Light 2 Ambient", &lightTwoAmbient, 0.0f, 1.0f);            
		ImGui::SliderFloat("Light 2 Diffuse", &lightTwoDiffuse, 0.0f, 1.0f);
		ImGui::SliderFloat("Light 2 Specular", &lightTwoSpecular, 0.0f, 1.0f);

		ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

		//if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
		//	counter++;
		//ImGui::SameLine();
		//ImGui::Text("counter = %d", counter);

		ImGui::InputFloat3("light pos 1", pointLightPositionsIG.at(0).data());
		ImGui::InputFloat3("light pos 2", pointLightPositionsIG.at(1).data());

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
		
		// don't forget to enable shader before setting uniforms
		objectShader.use();

		// set up lighting uniforms
		objectShader.setVec3("viewPos", camera.Position);
		objectShader.setFloat("shininess", 32.f);

		// point light 1
		plOne.pos = glm::make_vec3(pointLightPositionsIG.at(0).data());
		plOne.updateLight(glm::vec3(lightOneAmbient), glm::vec3(lightOneDiffuse), glm::vec3(lightOneSpecular));
		plOne.setShaderUniforms(objectShader, "pointLights[0]");
		
		// point light 2
		plTwo.pos = glm::make_vec3(pointLightPositionsIG.at(1).data());
		plTwo.updateLight(glm::vec3(lightTwoAmbient), glm::vec3(lightTwoDiffuse), glm::vec3(lightTwoSpecular));
		plTwo.setShaderUniforms(objectShader, "pointLights[1]");


		// point light 2
		/*objectShader.setVec3("pointLights[1].position", glm::make_vec3(pointLightPositionsIG.at(1).data()));
		objectShader.setVec3("pointLights[1].ambient", glm::vec3(0.25f));
		objectShader.setVec3("pointLights[1].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
		objectShader.setVec3("pointLights[1].specular", glm::vec3(1.0f, 1.0f, 1.0f));
		objectShader.setFloat("pointLights[1].constant", 1.0f);
		objectShader.setFloat("pointLights[1].linear", 0.09f);
		objectShader.setFloat("pointLights[1].quadratic", 0.012f);*/

		// view/projection transformations
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		objectShader.setMat4("projection", projection);
		objectShader.setMat4("view", view);

		// render the loaded model
		glm::mat4 model = glm::mat4(1.0f); // set to identity matrix
		model = glm::translate(model, glm::vec3(0.0f, -1.f, 0.0f)); // translate it down so it's at the center of the scene
		model = glm::scale(model, glm::vec3(0.13f));	
		objectShader.setMat4("model", model);
		ourModel.Draw(objectShader);
		
		// render the light proxies
		lightShader.use();
		lightShader.setMat4("projection", projection);
		lightShader.setMat4("view", view);

		for(const auto& lightpos : pointLightPositionsIG)
		{
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::make_vec3(lightpos.data()));
			model = glm::scale(model, glm::vec3(0.1f)); 
			lightShader.setMat4("model", model);
			sphereModel.Draw(lightShader);
		}

		// activate text shader and render text
		textShader.use();
		glm::mat4 textProjection = glm::ortho(0.f, static_cast<float>(SCR_HEIGHT), 0.f, static_cast<float>(SCR_WIDTH) / 2);
		textShader.setMat4("textProjection", textProjection);
		textObject.RenderText(textShader,"Use WASD to pan and zoom, press RMB and move mouse to move camera" , { 25.f, 20.f }, 0.2f, color::nrgb["sky blue"]);
		textObject.RenderText(textShader,
			"Camera xpos: " + fmt::format("{:.2f}", camera.Position.x) +
			" ypos: " + fmt::format("{:.2f}", camera.Position.y) +
			" zpos: " + fmt::format("{:.2f}", camera.Position.z),
			{ 25.f, 40.f }, 0.25f, { color::nrgb["tomato"] }
			);
		

		// IMGUI render
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		

		//____________________________
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	// IMGUI cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();


	glfwTerminate();
}