#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Shader.h"
#include "Camera.h"
#include "Mesh.h"
#include "Model.h"

#include<iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace std;

#pragma region Screen functions and Setting
// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xPos, double yPos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
#pragma endregion

#pragma region Camera Daclare and params
// camera
Camera camera(glm::vec3(0, 0, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;
#pragma endregion


int main(int argc, char* argv[])
{
	std::string exePath = argv[0];

	#pragma region Open a Window
		glfwInit();
		// OpenGL版本
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		// open GLFW Window
		GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Voxelize Test", NULL, NULL);

		if (window == NULL)
		{
			cout << "open window failed." << endl;
			glfwTerminate();
			return EXIT_FAILURE;
		}

		glfwMakeContextCurrent(window);
		glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
		//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		//glfwSetCursorPosCallback(window, mouse_callback);
		//glfwSetScrollCallback(window, scroll_callback);

		//Init GLEW
		glewExperimental = true;
		if (glewInit() != GLEW_OK)
		{
			cout << "glew init failed." << endl;
			glfwTerminate();
			return EXIT_FAILURE;
		}

		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
	#pragma endregion

	glEnable(GL_DEPTH_TEST);

	Shader* myShader = new Shader("vertexSource.vert", "fragmentSource.frag");

	#pragma region Init and Load Models
		Model model(exePath.substr(0, exePath.find_last_of('\\')) + "\\model\\nanosuit.obj");
	#pragma endregion

	// render loop
	while (!glfwWindowShouldClose(window))
	{
		#pragma region pre-frame time of Camera
			float currentFrame = glfwGetTime();
			deltaTime = currentFrame - lastFrame;
			lastFrame = currentFrame;
		#pragma endregion

		// Process input
		processInput(window); //套用(前一次迴圈的)用戶輸入

		// Clear Screen
		glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		myShader->use();

		#pragma region MVP matrices
				glm::mat4 modelMat;
				glm::mat4 viewMat;
				glm::mat4 projectMat;
				viewMat = camera.GetViewMatrix();
				projectMat = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / SCR_HEIGHT,
					0.1f, 100.0f);

				myShader->setMat4("viewMat", viewMat);
				myShader->setMat4("projectMat", projectMat);
		#pragma endregion

		/* Drawcall */
		// Set Model matrix
		modelMat = glm::mat4(1.0f);
		modelMat = glm::translate(modelMat, glm::vec3(0.0f, -1.75f, 0.0f));
		modelMat = glm::scale(modelMat, vec3(0.2, 0.2, 0.2));
		myShader->setMat4("modelMat", modelMat);
		// Set Model
		model.Draw(myShader);

		// Clean up, prepare for next render loop
		glfwPollEvents(); 
		glfwSwapBuffers(window);
	}

	// Exit program
	glfwTerminate();

	return 0;
}


void processInput(GLFWwindow* window)
{

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		camera.ProcessKeyboard(UP, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		camera.ProcessKeyboard(DOWN, deltaTime);

}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

// Whenever the mouse moves, this callback is call
void mouse_callback(GLFWwindow* window, double xPos, double yPos)
{
	if (firstMouse) {
		lastX = xPos;
		lastY = yPos;
		firstMouse = false;
	}
	float deltaX, deltaY;
	deltaX = xPos - lastX;
	deltaY = yPos - lastY;
	lastX = xPos;
	lastY = yPos;

	camera.ProcessMouseMovement(deltaX, deltaY);
}

// Whenever the mouse scroll wheel scrolls, this callback is call
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}