#include "GLLibMain.hpp"
#include <GLFW/glfw3.h>
#include <GL/glew.h>
#include <stdlib.h>
#include <stdio.h>

GLchar* g_pVertSrc = nullptr;
GLchar* g_pFragSrc = nullptr;

void* LoadTextFromFile(const char* filename, size_t* size) {
	size_t sz;
	FILE* fp;
	if (fp = fopen(filename, "r")) {
		fseek(fp, 0, SEEK_END);
		sz = ftell(fp);
		rewind(fp);

		void* data = calloc(sz, 1);
		if (data) {
			fread(data, sz, 1, fp);
			if (size) {
				*size = sz;
			}
			fclose(fp);
		}
		return data;
	}

	return nullptr;
}

void* LoadFromFile(const char* filename, size_t* size) {
	size_t sz;
	FILE* fp;
	if (fp = fopen(filename, "rb")) {
		fseek(fp, 0, SEEK_END);
		sz = ftell(fp);
		rewind(fp);

		void* data = malloc(sz);
		if (data) {
			fread(data, sz, 1, fp);
			if (size) {
				*size = sz;
			}
			fclose(fp);
		}
		return data;
	}

	return nullptr;
}

int main() {

	if (0 == glfwInit()) return -1;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* win = glfwCreateWindow(512, 512, " ", nullptr, nullptr);
	if (nullptr == win) return -1;
	glfwMakeContextCurrent(win);

	if (GLEW_OK != glewInit()) return -1;

	size_t sz_vert = 0, sz_frag = 0;
	g_pVertSrc = (GLchar*)LoadTextFromFile("transform2D.vert", &sz_vert);
	g_pFragSrc = (GLchar*)LoadTextFromFile("transform2D.frag", &sz_frag);


	Clb184::CShaderFactory factory;
	Clb184::CShaderProgram program;
	program.CreateProgram(
		factory.CreateShader(GL_VERTEX_SHADER, g_pVertSrc, 1),
		factory.CreateShader(GL_FRAGMENT_SHADER, g_pFragSrc, 1)
	);
}
