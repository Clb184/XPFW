#include "OpenGL/Window.h"
#include "Output.h"
#include <assert.h>


#ifdef WIN32 // Why does Microsoft do weird things? I'm forced to do this to make threads.h work
	#include <no_sal2.h>
#endif
#include <threads.h>

window_state_t DefaultWindowState() {
	window_state_t ret = { 0 };
	ret.fullscreen = false;
	ret.width = 640;
	ret.height = 480;
	ret.delta_draw = 60.0;
	ret.title = "OpenGL 4.6";
	return ret; // { false, 640, 480, "OpenGL 4.6" };
}

bool CreateGLWindow(const char* title, int width, int height, bool fullscreen, float delta_draw, window_t* window_data) {
	LOG_INFO("Creating Window with params (Creating State Struct)");
	assert(0 != window_data);

	window_state_t state = { 0 };
	state.fullscreen = fullscreen;
	state.width = width;
	state.height = height;
	state.delta_draw = delta_draw;
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
	mtx_t* draw_mutex;
	mtx_t* logic_mutex;
	void* data;
	float fps;
	bool on_exit;

	bool on_draw;
	bool on_move;
};

int _DrawLoopThreadFn(void* data) {
	struct draw_info_t* draw_info = (struct draw_info_t*)data;
	GLFWwindow* gl_win = draw_info->window_data->window;
	loop_fn draw_loop = draw_info->draw_fn;
	mtx_t draw_mutex;
	draw_info->draw_mutex = &draw_mutex;
	mtx_init(&draw_mutex, mtx_plain);


	// I'm using this thread to draw
	glfwMakeContextCurrent(gl_win);

	// Clear color
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0f);
	glfwSwapInterval(1); // Enable VSync

	while (!glfwWindowShouldClose(gl_win)) {
		while (draw_info->on_move) {
		}
		draw_info->on_move = false;
		draw_info->on_draw = true;
		glfwMakeContextCurrent(gl_win);


		// Clear Screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Draw function
		draw_loop(draw_info->window_data, draw_info->data);

		// Move the Swap Chain
		glfwSwapBuffers(gl_win);

		glfwMakeContextCurrent(0);
		draw_info->on_draw = false;
		draw_info->on_move = true;
	}

	draw_info->on_exit = true;
	return 0;
}

void RunMainLoop(window_t* window, void* data, loop_fn move_loop, loop_fn draw_loop) {
	// Show from icon, this could be done after loading everything
	glfwIconifyWindow(window->window);
	_sleep(1000); // Just wait a bit for the "restore window" (kinda imitating some game over there)
	glfwShowWindow(window->window);
	glfwRestoreWindow(window->window);

	const float delta_logic = 1.0 / 60.0;
	float logic_tick_acum = 0.0f;

	mtx_t logic_mutex;
	mtx_init(&logic_mutex, mtx_plain);

	thrd_t draw_thread;
	
	struct draw_info_t draw_info;
	draw_info.draw_fn = draw_loop;
	draw_info.window_data = window;
	draw_info.data = data;
	draw_info.fps = 0.0f;
	draw_info.on_exit = false;
	draw_info.logic_mutex = &logic_mutex;
	draw_info.on_move = true;
	draw_info.on_draw = false;

	float past_time = 0.0f;

	glfwMakeContextCurrent(0);
	thrd_create(&draw_thread, _DrawLoopThreadFn, &draw_info);
	_sleep(100);

	while (!draw_info.on_exit) {
		double temp = glfwGetTime();
		const float delta_time = temp - past_time;
		past_time = temp;
		while (draw_info.on_draw) {
		}
		draw_info.on_draw = false;
		draw_info.on_move = true;
		glfwMakeContextCurrent(window->window);
		// Process events and clear screen
		glfwPollEvents();
		glfwMakeContextCurrent(0);
		draw_info.on_move = false;
		draw_info.on_draw = true;

		// Run main loop
		// Move logic, fixed at 60 tps for convenience
		if (logic_tick_acum >= delta_logic) {
			while (draw_info.on_draw) {
			}
			draw_info.on_draw = false;
			draw_info.on_move = true;
			window->logic_acum = logic_tick_acum;
			move_loop(window, data);
			draw_info.on_move = false;
			draw_info.on_draw = true;
			logic_tick_acum = 0.0f;
		}
		logic_tick_acum += delta_time;
		_sleep(1);
	}
	int draw_res = 0;
}

void DestroyGLWindow(window_t* window) {
	// Destroy window and all related
	glfwDestroyWindow(window->window);
	window->window = 0;
	glfwTerminate();

}
