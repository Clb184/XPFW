#ifndef WINDOW_INCLUDED
#define WINDOW_INCLUDED

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define LOOP_FN(x) void x (window_t* window, float delta_time, void* data)

typedef struct {
	bool fullscreen; // = false;
	int width; // = 0;
	int height; // = 0;
	const char* title; // = "";
} window_state_t;

typedef struct {
	GLFWwindow* window; // = nullptr;
	void* data; // = nullptr;
	float fps; // = 0.0f;
	window_state_t window_state;
} window_t;

typedef void (*main_loop_fn) (window_t* window, float delta_time, void* data);

window_state_t DefaultWindowState();
bool CreateGLWindow(const char* title, int width, int height, bool fullscreen, window_t* window_data);
bool CreateGLWindowFromState(window_state_t state, window_t* window_data);
void RunMainLoop(window_t* window, void* data, main_loop_fn main_loop);
void DestroyGLWindow(window_t* window);

#ifdef __cplusplus
}
#endif

#endif