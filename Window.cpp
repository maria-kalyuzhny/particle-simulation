////////////////////////////////////////
// Window.cpp
////////////////////////////////////////

#include "Window.h"
#define M_PI 3.14159265

////////////////////////////////////////////////////////////////////////////////

// Window Properties
int Window::width;
int Window::height;
const char* Window::windowTitle = "CSE 169: Particle system Simulation";
GLFWwindow* Window::window;
ImVec2 Window::imguiMin;
ImVec2 Window::imguiMax;
bool Window::culling;

std::chrono::high_resolution_clock::time_point Window::start;
std::chrono::steady_clock::time_point Window::currTime;
bool Window::started;
bool Window::paused;
int Window::numIncr = 60;

// Objects
ParticleSystem * Window::ps;

// Physics constants
glm::vec3 Window::vWind = glm::vec3(0.0,0.0,0.0);
float Window::density = 1.225f;
float Window::drag = 1.5f;
float Window::kS = 15.0f;
float Window::kD = 0.4f;
float Window::mass = 1.5f;
float Window::restitution = 0.05f;
float Window::friction = 0.5f;
float Window::groundLevel = -0.5f;
glm::vec3 Window::gravity = glm::vec3(0.0,-1.0,0.0);

glm::vec3 Window::offset = glm::vec3(0);
glm::vec3 Window::rotation = glm::vec3(0);

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
	ps = new ParticleSystem(30, 30, 0.03, mass, kS, kD);
	/*std::cout << ps->vertices.size() << std::endl;
	std::cout << ps->vertices[0].x << ps->vertices[0].y << ps->vertices[0].z << std::endl;
	std::cout << ps->normals.size() << std::endl;
	std::cout << ps->normals[0].x << ps->normals[0].y << ps->normals[0].z << std::endl;
	std::cout << ps->faces.size() << std::endl;
	std::cout << ps->faces[0] << " " << ps->faces[1] << " " << ps->faces[2];*/
	ps->Update(Cam->GetViewProjectMtx(), Cam->GetViewMtx());

	return true;
}

void Window::cleanUp()
{
	// Deallocate the objects.
	delete ps;

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
		fsec sinceStart = currTime - start;
		float startDelta  = sinceStart.count();

		float delta = change/numIncr;
		ps->ApplyUserControls(offset, rotation);
		for (int i = 0; i < numIncr; i++) {
			ps->ApplyForces(gravity, vWind, density, drag);
			ps->IntegrateMotion(delta);
		}
		ps->ApplyConstraints(groundLevel, friction, restitution);
		ps->UpdateVertices();
		ps->UpdateNormals();
		ps->BindBuffers();
		ps->Update(Cam->GetViewProjectMtx(), Cam->GetViewMtx());
	}
	

	// Clear the color and depth buffers.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	

	// Render the object.
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


	std::string axes[3] = { "x", "y", "z" };
	
	ImGui::Text("ps position");
	for (int i = 0; i < 3; i++) {
		std::string slider_name = axes[i] + "##pos";
		ImGui::SliderFloat(slider_name.c_str(), &offset[i], -2.0f, 1.5f);
	}
	ImGui::Text("ps rotationn");
	for (int i = 0; i < 3; i++) {
		std::string slider_name = axes[i] + "##rot";
		ImGui::SliderFloat(slider_name.c_str(), &rotation[i], -M_PI, M_PI);
	}

	ImGui::Text(" ");
	ImGui::Text("PHYSICS");
	ImGui::Text("wind");
	for (int i = 0; i < 3; i++) {
		std::string slider_name = axes[i] + "##wind";
		ImGui::SliderFloat(slider_name.c_str(), &vWind[i], -0.5f, 0.5f);
	}




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