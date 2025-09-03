#include "OpenGL/Window.hpp"
#include "Output.hpp"
#include <assert.h>

window_state_t DefaultWindowState() {
	return { false, 640, 480, "OpenGL 4.6" };
}

bool CreateGLWindow(const char* title, int width, int height, bool fullscreen, main_loop_fn loop_fn, void* data, window_t* window_data) {
	LOG_INFO("Creating Window with params (Creating State Struct)");
	assert(nullptr != window_data);

	window_state_t state;
	state.fullscreen = fullscreen;
	state.width = width;
	state.height = height;
	state.title = title;
	window_data->data = data;
	return CreateGLWindowFromState(state, loop_fn, window_data);
}

bool CreateGLWindowFromState(window_state_t state, main_loop_fn loop_fn, window_t* window_data) {
	LOG_INFO("Creating Window with OpenGL 4.6 support with GLFW");
	assert(nullptr != window_data);

	// Start GLFW with Core OpenGL 4.6 support
	if (0 == glfwInit()) return -1;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwWindowHint(GLFW_AUTO_ICONIFY, GLFW_TRUE);
	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	GLFWwindow* win = glfwCreateWindow(state.width, state.height, state.title, state.fullscreen ? glfwGetPrimaryMonitor() : nullptr, nullptr);
	if (nullptr == win) { LOG_ERROR("Failed creating GLFW window"); return false; }
	glfwMakeContextCurrent(win);

	// Initialize GLEW
#ifdef WIN32
	if (GLEW_OK != glewInit()) { LOG_ERROR("Failed initializing GLEW"); return false; }
#elif defined linux
	// Linux is weird here, so we don't check it
	glewInit();
#endif

	window_data->window_state = state;
	window_data->main_loop = loop_fn;
	window_data->window = win;

	// Default config for what I usually use
	glDisable(GL_CULL_FACE);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthRange(-1.0f, 1.0f);
	return true;
}

void RunMainLoop(window_t* window) {
	// Show from icon, this could be done after loading everything
	glfwIconifyWindow(window->window);
	_sleep(1000);
	glfwShowWindow(window->window);
	glfwRestoreWindow(window->window);

	double past_time = 0.0f;
	glfwSwapInterval(1); // Actually vsync

	while (!glfwWindowShouldClose(window->window)) {
		double temp = glfwGetTime();
		const float delta_time = temp - past_time;
		past_time = temp;

		// Process events and clear screen
		glfwPollEvents();
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClearDepth(1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		window->fps = 1.0 / delta_time; // Set delta time, somewhat conveniently (?

		// Run main loop
		window->main_loop(delta_time, window->data);

		// Move the Swap Chain
		glfwSwapBuffers(window->window);
	}
}

void DestroyWindow(window_t* window) {
	glfwDestroyWindow(window->window);
	window->window = nullptr;
	glfwTerminate();

}