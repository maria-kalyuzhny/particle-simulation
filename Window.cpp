////////////////////////////////////////
// Window.cpp
////////////////////////////////////////

#include "Window.h"
#define M_PI 3.14159265

////////////////////////////////////////////////////////////////////////////////

// Window Properties
int Window::width;
int Window::height;
const char* Window::windowTitle = "CSE 169: Particle system";
GLFWwindow* Window::window;
ImVec2 Window::imguiMin;
ImVec2 Window::imguiMax;
bool Window::initGui;
bool Window::culling;

std::chrono::high_resolution_clock::time_point Window::start;
std::chrono::steady_clock::time_point Window::currTime;
bool Window::started;
bool Window::paused;
int Window::numIncr = 10;	// particle system steps numIncr times per
							// draw loop to avoid 'batches' of new particles

// Objects
ParticleSystem * Window::ps;
Cube * Window::ground;

// Physics constants
float Window::density = 1.225f;
float Window::drag = 0.1f;
float Window::mass = 0.1f;
float Window::restitution = 0.05f;
float Window::friction = 0.3f;
float Window::groundLevel = -2.0f;
glm::vec3 Window::gravity = glm::vec3(0.0,-9.8f,0.0);


// Camera Properties
Camera* Cam;

// Interaction Variables
bool LeftDown, RightDown;
int MouseX, MouseY;

// The shader program id
GLuint Window::shaderProgram;

////////////////////////////////////////////////////////////////////////////////

// Constructors and desctructors 
bool Window::initializeProgram() {

	// Create a shader program with a vertex shader and a fragment shader.
	shaderProgram = LoadShaders("shaders/shader.vert", "shaders/shader.frag");

	// Check the shader program.
	if (!shaderProgram)
	{
		std::cerr << "Failed to initialize shader program" << std::endl;
		return false;
	}

	return true;
}

bool Window::initializeObjects()
{
	started = false;
	culling = true;
	initGui = false;
	ps = new ParticleSystem();
	ground = new Cube(glm::vec3(-100, -100, -100), glm::vec3(100, groundLevel-0.1, 100));
	return true;
}

void Window::cleanUp()
{
	// Deallocate the objects.
	delete ps;
	delete ground;

	// Delete the shader program.
	glDeleteProgram(shaderProgram);
}

////////////////////////////////////////////////////////////////////////////////

// for the Window
GLFWwindow* Window::createWindow(int width, int height)
{
	// Initialize GLFW.
	if (!glfwInit())
	{
		std::cerr << "Failed to initialize GLFW" << std::endl;
		return NULL;
	}

	// 4x antialiasing.
	glfwWindowHint(GLFW_SAMPLES, 4);

#ifdef __APPLE__ 
	// Apple implements its own version of OpenGL and requires special treatments
	// to make it uses modern OpenGL.

	// Ensure that minimum OpenGL version is 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Enable forward compatibility and allow a modern OpenGL context
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// Create the GLFW window.
	//GLFWwindow* window = glfwCreateWindow(width, height, windowTitle, NULL, NULL);
	window = glfwCreateWindow(width, height, windowTitle, NULL, NULL);

	// Check if the window could not be created.
	if (!window)
	{
		std::cerr << "Failed to open GLFW window." << std::endl;
		glfwTerminate();
		return NULL;
	}

	// Make the context of the window.
	glfwMakeContextCurrent(window);

#ifndef __APPLE__
	// On Windows and Linux, we need GLEW to provide modern OpenGL functionality.

	// Initialize GLEW.
	if (glewInit())
	{
		std::cerr << "Failed to initialize GLEW" << std::endl;
		return NULL;
	}
#endif

	// Set swap interval to 1.
	glfwSwapInterval(0);

	// set up the camera
	Cam = new Camera();
	Cam->SetAspect(float(width) / float(height));

	// initialize the interaction variables
	LeftDown = RightDown = false;
	MouseX = MouseY = 0;

	// Call the resize callback to make sure things get drawn immediately.
	Window::resizeCallback(window, width, height);

	return window;
}

void Window::resizeCallback(GLFWwindow* window, int width, int height)
{
#ifdef __APPLE__
	glfwGetFramebufferSize(window, &width, &height); 
#endif
	Window::width = width;
	Window::height = height;
	// Set the viewport size.
	glViewport(0, 0, width, height);

	Cam->SetAspect(float(width) / float(height));
}

////////////////////////////////////////////////////////////////////////////////

// update and draw functions
void Window::idleCallback()
{
	if (paused) {
		return;
	}

	// Perform any updates as necessary. 
	Cam->Update();
}

void Window::displayCallback(GLFWwindow* window)
{	
	if (!started) {
		currTime = std::chrono::high_resolution_clock::now();
		start= std::chrono::high_resolution_clock::now();
		started = true;
	}
	else {
		auto prevTime = currTime;
		currTime = std::chrono::high_resolution_clock::now();
		fsec changeFsec = currTime - prevTime;
		float change = changeFsec.count();
		float delta = change/numIncr;

		//std::cout << delta*100 << std::endl;
		for (int i = 0; i < numIncr; i++) {
			ps->ApplyForces(gravity, density, drag);
			ps->Step(delta);
			ps->ApplyConstraints(groundLevel, restitution, friction);
		}
		ps->UpdateVertices();
		ps->BindBuffers();
		ps->Update(Cam->GetViewProjectMtx(), Cam->GetViewMtx());
	}
	
	// Clear the color and depth buffers.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	

	// Render the object.
	ground->draw(Cam->GetViewProjectMtx(), Window::shaderProgram);
	ps->Draw(Window::shaderProgram);
	drawImgui();

	// Gets events, including input such as keyboard and mouse or window resizing.
	glfwPollEvents();
	// Swap buffers.
	glfwSwapBuffers(window);
}

// helper to set correct imgui window position min and max
void Window::updateImguiPos() {
	// hack to include borders and scrollbar in desired rect
	int smallPadding = 10;
	int scrollPadding = 30;
	int titlePadding = 40;
	imguiMin = ImGui::GetWindowContentRegionMin();
	imguiMax = ImGui::GetWindowContentRegionMax();

	imguiMin.x += ImGui::GetWindowPos().x - smallPadding;
	imguiMin.y += ImGui::GetWindowPos().y + ImGui::GetScrollY() - titlePadding;
	imguiMax.x += ImGui::GetWindowPos().x + scrollPadding;
	imguiMax.y += ImGui::GetWindowPos().y + ImGui::GetScrollY() + smallPadding;

	// rectangle for debugging
	//ImGui::GetForegroundDrawList()->AddRect(imguiMin, imguiMax, IM_COL32(255, 255, 0, 255));
}

void Window::drawImgui() {
	// start new frame
	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	//ImGui::SetWindowFontScale(10.0f);
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImVec2(Window::width / 3, Window::height));

	ImGui::Begin("Particle system simulation");
	ImGui::SetWindowFontScale(2.0f);

	if (ImGui::Button("Reset defaults"))
	{
		density = 1.225f;
		drag = 0.1f;
		mass = 0.0f;
		restitution = 0.05f;
		friction = 0.5f;
		gravity = glm::vec3(0.0, -9.8f, 0.0);
		ps->startPos = glm::vec3(0.0f, 1.0f, 0.0f);
		ps->startVel = glm::vec3(0.0f, 1.0f, 0.0f);
		ps->lifeSpan = 3.0f;
		ps->posVar = 1.0f;
		ps->velVar = 1.0f;
		ps->lifeVar = 2.0f;
		ps->pps = 50;
		ps->radius = 0.1f;
	}

	// Create slider hierarchy
	std::string slider_name;
	std::string axes[3] = { "x", "y", "z" };
	if (!initGui) ImGui::SetNextTreeNodeOpen(true);
	if (ImGui::TreeNode("Particle controls")) {
		//if (!initGui) ImGui::SetNextTreeNodeOpen(true);
		if (ImGui::TreeNode("position")) {
			for (int i = 0; i < 3; i++) {
				std::string slider_name = axes[i] + "##pos";
				if (i == 1) {
					ImGui::SliderFloat(slider_name.c_str(), &ps->startPos[i], groundLevel+0.001, 5.0f);
				}
				else {
					ImGui::SliderFloat(slider_name.c_str(), &ps->startPos[i], -5.0f, 5.0f);
				}
			}
			slider_name = "var##pos";
			ImGui::SliderFloat(slider_name.c_str(), &ps->posVar, 0.0f, 5.0f);
			ImGui::TreePop();
		}
		//if (!initGui) ImGui::SetNextTreeNodeOpen(true);
		if (ImGui::TreeNode("velocity")) {
			for (int i = 0; i < 3; i++) {
				std::string slider_name = axes[i] + "##rot";
				ImGui::SliderFloat(slider_name.c_str(), &ps->startVel[i], -10.0f, 10.0f);
			}
			slider_name = "var##vel";
			ImGui::SliderFloat(slider_name.c_str(), &ps->velVar, 0.0f, 10.0f);
			ImGui::TreePop();
		}
		if (!initGui) ImGui::SetNextTreeNodeOpen(true);
		if (ImGui::TreeNode("radius")) {
			slider_name = "##radius";
			ImGui::SliderFloat(slider_name.c_str(), &ps->radius, 0.01f, 2.0f);
			ImGui::TreePop();
		}
		if (!initGui) ImGui::SetNextTreeNodeOpen(true);
		if (ImGui::TreeNode("life span")) {
			slider_name = "span##life";
			ImGui::SliderFloat(slider_name.c_str(), &ps->lifeSpan, 0.001f, 10.0f);
			slider_name = "var##life";
			ImGui::SliderFloat(slider_name.c_str(), &ps->lifeVar, 0.0f, 10.0f);
			ImGui::TreePop();
		}
		if (!initGui) ImGui::SetNextTreeNodeOpen(true);
		if (ImGui::TreeNode("particles per second")) {
			slider_name = "##pps";
			ImGui::SliderFloat(slider_name.c_str(), &ps->pps, 1.0f, 100.0f);
			ImGui::TreePop();
		}
		//if (!initGui) ImGui::SetNextTreeNodeOpen(true);
		if (ImGui::TreeNode("color")) {
			for (int i = 0; i < 3; i++) {
				std::string slider_name = axes[i] + "##color";
				ImGui::SliderFloat(slider_name.c_str(), &ps->color[i], 0.0f, 1.0f);
			}
			ImGui::TreePop();
		}
		ImGui::TreePop();
	}
	if (!initGui) ImGui::SetNextTreeNodeOpen(true);
	if (ImGui::TreeNode("Environment controls")) {
		if (!initGui) ImGui::SetNextTreeNodeOpen(true);
		if (ImGui::TreeNode("gravity")) {
			slider_name = "##gravity";
			ImGui::SliderFloat(slider_name.c_str(), &gravity.y, 0.0f, -20.0f);
			ImGui::TreePop();
		}
		if (!initGui) ImGui::SetNextTreeNodeOpen(true);
		if (ImGui::TreeNode("drag coefficient")) {
			slider_name = "##drag";
			ImGui::SliderFloat(slider_name.c_str(), &drag, 0.0f, 5.0f);
			ImGui::TreePop();
		}
		if (!initGui) ImGui::SetNextTreeNodeOpen(true);
		if (ImGui::TreeNode("density")) {
			slider_name = "##density";
			ImGui::SliderFloat(slider_name.c_str(), &density, 0.0f, 5.0f);
			ImGui::TreePop();
		}
		if (!initGui) ImGui::SetNextTreeNodeOpen(true);
		if (ImGui::TreeNode("collision restitution")) {
			slider_name = "##restitution";
			ImGui::SliderFloat(slider_name.c_str(), &restitution, 0.001f, 1.0f);
			ImGui::TreePop();
		}
		if (!initGui) ImGui::SetNextTreeNodeOpen(true);
		if (ImGui::TreeNode("collision friction")) {
			slider_name = "##friction";
			ImGui::SliderFloat(slider_name.c_str(), &friction, 0.0f, 1.0f);
			ImGui::TreePop();
		}
		ImGui::TreePop();
	}

	initGui = true;
	updateImguiPos();
	ImGui::End();

	// Render imgui onto screen
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

////////////////////////////////////////////////////////////////////////////////

// helper to reset the camera
void Window::resetCamera() 
{
	Cam->Reset();
	Cam->SetAspect(float(Window::width) / float(Window::height));
}

////////////////////////////////////////////////////////////////////////////////

// callbacks - for Interaction 
void Window::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	
	// Check for a key press.
	if (action == GLFW_PRESS)
	{
		switch (key) 
		{
		case GLFW_KEY_ESCAPE:
			// Close the window. This causes the program to also terminate.
			glfwSetWindowShouldClose(window, GL_TRUE);				
			break;

		case GLFW_KEY_R:
			resetCamera();
			break;
		case GLFW_KEY_C:
			if (culling){
				glDisable(GL_CULL_FACE);
				culling = false;
			}
			else {
				glEnable(GL_CULL_FACE);
				culling = true;
			}
			break;
		case GLFW_KEY_P:
			if (!paused) {
				paused = true;
			}
			else if (paused) {
				paused = false;
			}
			break;
		default:
			break;
		}
	}
}

void Window::mouse_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		LeftDown = (action == GLFW_PRESS);
	}
	if (button == GLFW_MOUSE_BUTTON_RIGHT) {
		RightDown = (action == GLFW_PRESS);
	}
}

void Window::cursor_callback(GLFWwindow* window, double currX, double currY) {
	int maxDelta = 100;
	int dx = glm::clamp((int)currX - MouseX, -maxDelta, maxDelta);
	int dy = glm::clamp(-((int)currY - MouseY), -maxDelta, maxDelta);

	MouseX = (int)currX;
	MouseY = (int)currY;

	// don't update rotation if mouse inside Imgui window
	if (MouseX >= imguiMin.x && MouseX <= imguiMax.x &&
		MouseY >= imguiMin.y && MouseY <= imguiMax.y )
	{
		return;
	}

	// don't update rotation if mouse outside window
	if (MouseX < 0 || MouseX > Window::width || 
		MouseY < 0 || MouseY > Window::height)
	{
		return;
	}

	// Move camera
	// NOTE: this should really be part of Camera::Update()
	if (LeftDown) {
		const float rate = 1.0f;
		Cam->SetAzimuth(Cam->GetAzimuth() + dx * rate);
		Cam->SetIncline(glm::clamp(Cam->GetIncline() - dy * rate, -90.0f, 90.0f));
	}
	if (RightDown) {
		const float rate = 0.005f;
		float dist = glm::clamp(Cam->GetDistance() * (1.0f - dx * rate), 0.01f, 1000.0f);
		Cam->SetDistance(dist);
	}
}

////////////////////////////////////////////////////////////////////////////////