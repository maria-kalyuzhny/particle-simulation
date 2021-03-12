#ifndef _WINDOW_H_
#define _WINDOW_H_

#include <chrono>
#include "main.h"
#include "Cube.h"
#include "shader.h"
#include "Camera.h"
#include "ParticleSystem.h"

////////////////////////////////////////////////////////////////////////////////

class Window
{
public:
	// Window Properties
	static int width;
	static int height;
	static const char* windowTitle;
	static GLFWwindow* window;
	static bool culling;

	// time controls
	static std::chrono::high_resolution_clock::time_point start;
	static std::chrono::steady_clock::time_point currTime;
	static bool started;
	static bool paused;
	static int numIncr;

	// Imgui display dimensions
	static ImVec2 imguiMin;
	static ImVec2 imguiMax;

	// Objects
	static ParticleSystem* ps;
	static Cube* ground;

	// Physics constants
	static glm::vec3 gravity;
	static float density;
	static float drag;
	static float mass;
	static float groundLevel;
	static float restitution;
	static float friction;

	// Shader Program 
	static GLuint shaderProgram;

	// Act as Constructors and desctructors 
	static bool initializeProgram();
	static bool initializeObjects();
	static void cleanUp();

	// for the Window
	static GLFWwindow* createWindow(int width, int height);
	static void resizeCallback(GLFWwindow* window, int width, int height);

	// GUI helpers
	static void updateImguiPos();
	static void drawImgui();

	// update and draw functions
	static void idleCallback();
	static void displayCallback(GLFWwindow*);

	// helper to reset the camera
	static void resetCamera();

	// callbacks - for interaction
	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void mouse_callback(GLFWwindow* window, int button, int action, int mods);
	static void cursor_callback(GLFWwindow* window, double currX, double currY);

	typedef std::chrono::duration<float> fsec;
};

////////////////////////////////////////////////////////////////////////////////

#endif