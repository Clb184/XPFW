#include "OpenGL/Window.h"
#include "Output.h"
#include <assert.h>

#ifdef __linux__
#include <unistd.h>
#endif

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

void GLDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* msg, const void* user_data) {
	fprintf(stdout, "GL MSG: src %d type: %d id %d severity %d \"%s\"\n", source, type, id, severity, msg);
}

bool CreateGLWindowFromState(window_state_t state, window_t* window_data) {
	char buf[512];
	sprintf(buf, "Creating Window with OpenGL 4.5 support with GLFW (%d x %d) (%s)", state.width, state.height, state.fullscreen ? "Fullscreen" : "Windowed");
	LOG_INFO(buf);
	assert(0 != window_data);

	// Start GLFW with Core OpenGL 4.5 support because I want to support as many machines as possible
	if (GLFW_FALSE == glfwInit()) return -1;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwWindowHint(GLFW_AUTO_ICONIFY, GLFW_TRUE);
	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	GLFWwindow* win = glfwCreateWindow(state.width, state.height, state.title, state.fullscreen ? glfwGetPrimaryMonitor() : 0, 0);
	if (0 == win) { LOG_ERROR("Failed creating GLFW window"); return false; }


	glfwMakeContextCurrent(win);
	glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

	// Initialize GLEW
	LOG_INFO("Initializing GLEW");
#ifdef WIN32
	if (GLEW_OK != glewInit()) { LOG_ERROR("Failed initializing GLEW"); return false; }
#elif defined linux
	// Linux is weird here, so don't check it
	glewInit();
#endif
	sprintf(buf, "Vendor: %s", glGetString(GL_VENDOR));
	LOG_INFO(buf);
	sprintf(buf, "GPU: %s", glGetString(GL_RENDERER));
	LOG_INFO(buf);
	sprintf(buf, "OpenGL Version: %s", glGetString(GL_VERSION));
	LOG_INFO(buf);
	sprintf(buf, "GLSL Version: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));
	LOG_INFO(buf);

	//glDebugMessageCallback(GLDebugCallback, 0);

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
	glViewport(0.0f, 0.0f, state.width, state.height);
	return true;
}

struct draw_info_t {
	move_loop_fn draw_fn;
	window_t* window_data;
	void* data;
	float fps;
	double delta_draw;
	double delta_move;
};

void RunMainLoop(window_t* window, void* data, move_loop_fn move_loop, draw_loop_fn draw_loop) {
	// Show window
	glfwShowWindow(window->window);

	const double delta_logic = 1.0 / 60.0;
	const double delta_draw = 1.0 / 60.0;
	double logic_tick_acum = 0.0f;
	double draw_tick_acum = 0.0f;

	/*struct draw_info_t draw_info;
	draw_info.draw_fn = draw_loop;
	draw_info.window_data = window;
	draw_info.data = data;
	draw_info.fps = 0.0f;*/

	GLFWwindow* win = window->window;

	//window->__internal = &draw_info;

	// Clear color
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0f);

	// Enable VSync
	glfwSwapInterval(0);

	double past_time = 0.0f;

	bool on_sleep = false;
	while (!glfwWindowShouldClose(window->window)) {
		// Process events and clear screen
		glfwPollEvents();

		double temp = glfwGetTime();

		// Run main loop
		// Move logic, fixed at 60 tps for convenience
		if (logic_tick_acum >= delta_logic) {

			window->delta_time = logic_tick_acum;
			//draw_info.delta_move = logic_tick_acum;
			move_loop(window, data);
			logic_tick_acum = 0.0f;
			on_sleep = true;
		}

		if (draw_tick_acum >= delta_draw) {

			// Clear Screen
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// Draw function
			window->delta_draw = draw_tick_acum;
			draw_loop(window, data);

			// Move the Swap Chain
			glfwSwapBuffers(win);
			draw_tick_acum = 0.0;
			//_sleep(1);
		}

		const double delta_time = temp - past_time;
		past_time = temp;
		logic_tick_acum += delta_time;
		draw_tick_acum += delta_time;
	}

	// Doing this only for convenience
	DestroyGLWindow(window);
}

void RunMainLoopDT(window_t* window, void* data, move_loop_dt_fn move_loop, draw_loop_dt_fn draw_loop) {
	// Show window
	glfwShowWindow(window->window);

	const double delta_logic = 1.0 / 60.0;
	const double delta_draw = 1.0 / 60.0;
	double logic_tick_acum = 0.0f;
	double draw_tick_acum = 0.0f;

	/*struct draw_info_t draw_info;
	draw_info.draw_fn = draw_loop;
	draw_info.window_data = window;
	draw_info.data = data;
	draw_info.fps = 0.0f;*/

	GLFWwindow* win = window->window;

	//window->__internal = &draw_info;

	// Clear color
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0f);

	// Enable VSync
	glfwSwapInterval(1);

	double past_time = 0.0f;
	double temp = 0.0f;

	bool on_sleep = false;
	while (!glfwWindowShouldClose(window->window)) {
		// Process events and clear screen
		glfwPollEvents();

		temp = glfwGetTime();
		const double delta_time = temp - past_time;
		past_time = temp;
		//printf("temp: %.4f\n", temp);
		//printf("delt: %.4f\n", delta_time);
		//printf("past: %.4f\n", past_time);

		// Run main loop
		// Move logic, uncapped to whatever refresh rate the display has
		window->delta_time = delta_time;
		move_loop(window, delta_time, data);
		on_sleep = true;

		// Clear Screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Draw function
		window->delta_draw = delta_time;
		draw_loop(window, delta_time, data);

		// Move the Swap Chain
		glfwSwapBuffers(win);
		
	}

	// Doing this only for convenience
	DestroyGLWindow(window);
}

float GetWindowFPS(window_t* window) {
	return 1.0 / window->delta_draw;
}

float GetWindowTPS(window_t* window) {
	return 1.0 / window->delta_time;
}

void DestroyGLWindow(window_t* window) {
	// Destroy window and all related
	glfwDestroyWindow(window->window);
	window->window = 0;
	glfwTerminate();

}
