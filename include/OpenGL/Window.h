#ifndef WINDOW_INCLUDED
#define WINDOW_INCLUDED

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	bool fullscreen; // = false;
	int width; // = 0;
	int height; // = 0;
	const char* title; // = "";
} window_state_t;

typedef void (*main_loop_fn) (float delta_time, void* data);

typedef struct {
	GLFWwindow* window; // = nullptr;
	void* data; // = nullptr;
	float fps; // = 0.0f;
	window_state_t window_state;
	main_loop_fn main_loop;
} window_t;


window_state_t DefaultWindowState();
bool CreateGLWindow(const char* title, int width, int height, bool fullscreen, main_loop_fn loop_fn, void* data, window_t* window_data);
bool CreateGLWindowFromState(window_state_t state, main_loop_fn loop_fn, window_t* window_data);
void RunMainLoop(window_t* window);
void DestroyGLWindow(window_t* window);

#ifdef __cplusplus
}
#endif

#endif