#include "OpenGL/Window.h"
#include "Output.h"
#include <assert.h>

window_state_t DefaultWindowState() {
	window_state_t ret = { 0 };
	ret.fullscreen = false;
	ret.width = 640;
	ret.height = 480;
	ret.title = "OpenGL 4.6";
	return ret; // { false, 640, 480, "OpenGL 4.6" };
}

bool CreateGLWindow(const char* title, int width, int height, bool fullscreen, window_t* window_data) {
	LOG_INFO("Creating Window with params (Creating State Struct)");
	assert(0 != window_data);

	window_state_t state = { 0 };
	state.fullscreen = fullscreen;
	state.width = width;
	state.height = height;
	state.title = title;
	return CreateGLWindowFromState(state, window_data);
}

bool CreateGLWindowFromState(window_state_t state, window_t* window_data) {
	LOG_INFO("Creating Window with OpenGL 4.6 support with GLFW");
	assert(0 != window_data);

	// Start GLFW with Core OpenGL 4.6 support
	if (0 == glfwInit()) return -1;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwWindowHint(GLFW_AUTO_ICONIFY, GLFW_TRUE);
	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	GLFWwindow* win = glfwCreateWindow(state.width, state.height, state.title, state.fullscreen ? glfwGetPrimaryMonitor() : 0, 0);
	if (0 == win) { LOG_ERROR("Failed creating GLFW window"); return false; }
	glfwMakeContextCurrent(win);

	// Initialize GLEW
#ifdef WIN32
	if (GLEW_OK != glewInit()) { LOG_ERROR("Failed initializing GLEW"); return false; }
#elif defined linux
	// Linux is weird here, so don't check it
	glewInit();
#endif

	window_data->window_state = state;
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

struct draw_info_t {
	loop_fn draw_fn;
	window_t* window_data;
	void* data;
	float fps;
	double delta_draw;
	double delta_move;
};

void RunMainLoop(window_t* window, void* data, loop_fn move_loop, loop_fn draw_loop) {
	// Show from icon, this could be done after loading everything
	glfwIconifyWindow(window->window);
	_sleep(1000); // Just wait a bit for the "restore window" (kinda imitating some game over there)
	glfwShowWindow(window->window);
	glfwRestoreWindow(window->window);

	const double delta_logic = 1.0 / 60.0;
	const double delta_draw = 1.0 / 60.0;
	double logic_tick_acum = 0.0f;
	double draw_tick_acum = 0.0f;

	struct draw_info_t draw_info;
	draw_info.draw_fn = draw_loop;
	draw_info.window_data = window;
	draw_info.data = data;
	draw_info.fps = 0.0f;

	GLFWwindow* win = window->window;

	window->__internal = &draw_info;

	// Clear color
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0f);

	// Enable VSync
	glfwSwapInterval(1);

	float past_time = 0.0f;


	while (!glfwWindowShouldClose(window->window)) {
		// Process events and clear screen
		glfwPollEvents();

		double temp = glfwGetTime();

		// Run main loop
		// Move logic, fixed at 60 tps for convenience
		if (logic_tick_acum >= delta_logic) {

			window->delta_time = logic_tick_acum;
			draw_info.delta_move = logic_tick_acum;
			move_loop(window, data);
			logic_tick_acum = 0.0f;
		}

		if (draw_tick_acum >= delta_draw) {

			// Clear Screen
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// Draw function
			draw_info.delta_draw = draw_tick_acum;
			draw_loop(draw_info.window_data, draw_info.data);

			// Move the Swap Chain
			glfwSwapBuffers(win);
			draw_tick_acum = 0.0;
		}

		const double delta_time = temp - past_time;
		past_time = temp;
		logic_tick_acum += delta_time;
		draw_tick_acum += delta_time;
	}

	// Doing this only for convenience
	DestroyGLWindow(window);
}

float GetWindowFPS(window_t* window) {
	return 1.0 / ((struct draw_info_t*)window->__internal)->delta_draw;
}

float GetWindowTPS(window_t* window) {
	return 1.0 / ((struct draw_info_t*)window->__internal)->delta_move;
}

void DestroyGLWindow(window_t* window) {
	// Destroy window and all related
	glfwDestroyWindow(window->window);
	window->window = 0;
	glfwTerminate();

}
