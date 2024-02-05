// Taken from https://learnopengl.com/code_viewer_gh.php?code=src/1.getting_started/1.2.hello_window_clear/hello_window_clear.cpp and modified to work with glfw
// All credit go to Joey De Vries: https://joeydevries.com/#home author of Learn OpenGL.
// Modified by: Jakob Törmä Ruhl
// Functionality extended by Síofra McCarthy

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GL\glew.h>
#include <GLFW/glfw3.h>

#include <chrono>
#include <vector>
#include <iostream>

#define GRAVITAIONAL_FORCE 96
#define REFRACTORY_TIME 125
#define BALL_SIZE 8

#define FPS_60_FRAME_TIME 1.f / 60.f

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

bool debug = true;
std::chrono::steady_clock::time_point timeCurrent;
std::chrono::steady_clock::time_point timeLastFrame;
std::chrono::duration<double, std::milli> delta;
float frames = 0.f;
float deltaTime = 0.f;

struct Ball {
	ImVec2 posCenter;
	float radius;
	ImVec2 velocity;
	uint32_t color;

	Ball(ImVec2 cen, float r, ImVec2 v, uint32_t col = 0xffff00ff) {
		posCenter = cen;
		radius = r;
		velocity = v;
		color = col;
	}

	void move(ImVec2 dist) {
		posCenter.x += dist.x;
		posCenter.y += dist.y;
	}

	void update(float timeDelta) {
		//gravity 
		velocity.y += timeDelta * GRAVITAIONAL_FORCE;

		ImVec2 moveDist = { timeDelta * velocity.x, timeDelta * velocity.y };
		move(moveDist);
	}
};

std::vector<Ball*> balls;

class BallShooter {
	ImVec2 position;
	ImVec2 exitVelocity;

	float timeSinceLastShot = REFRACTORY_TIME;

	void shoot() {
		balls.push_back(new Ball(position, BALL_SIZE, exitVelocity));
	}
public:
	BallShooter(ImVec2 p, ImVec2 dir, float mag) {
		position = p;
		exitVelocity = ImVec2(mag * dir.x, mag * dir.y);
	}

	void update(float timeDelta) {
		//if (timeDelta > FPS_60_FRAME_TIME) return;
		timeSinceLastShot += timeDelta;
		if (timeSinceLastShot > REFRACTORY_TIME) {
			shoot();
			timeSinceLastShot = 0.f;
		}
	}
};


void showBallsWindow() {
	ImGui::Begin("balls");
	for (Ball* ball : balls) {
		ImVec2 posWindowOffset = ImGui::GetWindowPos();
		ImVec2 posBallOffset = { ball->posCenter.x + posWindowOffset.x, ball->posCenter.y + posWindowOffset.y };
		ImGui::GetWindowDrawList()->AddCircleFilled(posBallOffset, ball->radius, ball->color);
		if (ball->posCenter.y + ball->radius > ImGui::GetWindowHeight()) {
			ball->velocity.y *= -.9;
			ball->posCenter.y = ImGui::GetWindowHeight() - ball->radius;
		}
		if (ball->posCenter.y - ball->radius < 0) {
			ball->velocity.y *= -.9;
			ball->posCenter.y = ball->radius;
		}
		if (ball->posCenter.x + ball->radius > ImGui::GetWindowWidth()) {
			ball->velocity.x *= -.9;
			ball->posCenter.x = ImGui::GetWindowWidth() - ball->radius;
		}
		if (ball->posCenter.x - ball->radius < 0) {
			ball->velocity.x *= -.9;
			ball->posCenter.x = ball->radius;
		}
	}

	ImGui::End();
}

void showDebugWindow() {
	ImGui::Begin("debug");
	ImGui::Text("Frames: %d", (int)frames);
	ImGui::Text("Objects: %d", balls.size());
	ImGui::Text("Time Delta: %f", deltaTime);
	ImGui::End();
}

void handleCollisions() {
	for (Ball* ball1 : balls) {
		for (Ball* ball2 : balls) {
			if (ball1 != ball2) {
				float dx = ball2->posCenter.x - ball1->posCenter.x;
				float dy = ball2->posCenter.y - ball1->posCenter.y;
				float dist = sqrt(dx * dx + dy * dy);
				if (dist < ball1->radius + ball2->radius) {
					ball1->move({ dx/-2, dy/-2 });
					ball2->move({ dx / 2, dy / 2 });
				}
			}
		}
	}
}

int main()
{
	BallShooter* shooter = new BallShooter({ 50, 50 }, { 1, 0 }, 500);

	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	//init glew after the context have been made
	glewInit();


	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // IF using Docking Branch

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
	ImGui_ImplOpenGL3_Init();

	// render loop
	// -----------
	timeLastFrame = timeCurrent = std::chrono::steady_clock::now();
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		// input
		// -----
		processInput(window);

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		showBallsWindow();
		if (debug) showDebugWindow();

		// render
		// ------
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Rendering
		// (Your code clears your framebuffer, renders your other stuff etc.)
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// Update and Render additional Platform Windows
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();

	
		timeCurrent = std::chrono::steady_clock::now();
		delta = timeCurrent - timeLastFrame;
		deltaTime = delta.count();
		frames = 1.f / deltaTime;

		timeLastFrame = timeCurrent;

		shooter->update(deltaTime);

		for (auto ball : balls) {
			ball->update(deltaTime / 1000);
		}

		handleCollisions();

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
	}

	for (Ball* ball : balls) {
		delete ball;
	}

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}
