#include "GLLibMain.hpp"
#include <GLFW/glfw3.h>
#include <GL/glew.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

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
	g_pVertSrc = (GLchar*)LoadTextFromFile("transform3D.vert", &sz_vert);
	g_pFragSrc = (GLchar*)LoadTextFromFile("transform3D.frag", &sz_frag);

	Clb184::CShaderFactory factory;
	Clb184::CShaderProgram program;
	GLuint vrt = factory.CreateShader(GL_VERTEX_SHADER, g_pVertSrc, 1);
	GLuint frg = factory.CreateShader(GL_FRAGMENT_SHADER, g_pFragSrc, 1);
	while (GLenum e = glGetError()) {
		printf("OpenGL Error %d\n", e);
	}
	if(false == program.CreateProgram(vrt, frg)) return -1;
	while (GLenum e = glGetError()) {
		printf("OpenGL Error %d\n", e);
	}

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glDepthFunc(GL_GEQUAL);
	glDepthMask(GL_TRUE);
	glDepthRange(1.0f, -1.0f);

	while (GLenum e = glGetError()) {
		printf("OpenGL Error %d\n", e);
	}

	Clb184::TLVertex3D verts[] = {
		{ 0.0f, 0.0f, 0.0f,   0xff0000ff,   0.0f, 0.0f,   0.0f, 1.0f, 0.0f },
		{ 1.0f, 0.0f, 0.0f,   0xffffff00,   0.0f, 0.0f,   0.0f, 1.0f, 0.0f },
		{ 0.0f, 1.0f, 1.0f,   0xffff0000,   0.0f, 0.0f,   0.0f, 1.0f, 0.0f },

		{ 0.0f, 0.0f, 1.0f,   0x00ffffff,   0.0f, 0.0f,   0.0f, 1.0f, 0.0f },
		{ 1.0f, 0.0f, 0.0f,   0x0000ff00,   0.0f, 0.0f,   0.0f, 1.0f, 0.0f },
		{ 0.0f, 1.0f, 0.0f,   0xffffffff,   0.0f, 0.0f,   0.0f, 1.0f, 0.0f },
	};

	Clb184::CVertexBuffer vbuffer;
	Clb184::CVertexAttribute vattribute;
	vbuffer.Create(sizeof(verts), verts, GL_STATIC_DRAW);
	vattribute.Create();
	vattribute.SetAttributeData(Clb184::g_TL3DAttributes, Clb184::TL3DAttributeCount);

	while (GLenum e = glGetError()) {
		printf("OpenGL Error %d\n", e);
	}

	float identity[] = {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f,
	};
	program.Bind();
	while (GLenum e = glGetError()) {
		printf("OpenGL Error %d\n", e);
	}

	struct draw_cmd_t {
		GLuint count = sizeof(verts) / sizeof(Clb184::TLVertex3D);
		GLuint instance_cnt = 1;
		GLuint first = 0;
		GLuint base = 0;
	} draw_cmd;


	GLuint draw_buffer_cmd = -1;
	glCreateBuffers(1, &draw_buffer_cmd);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, draw_buffer_cmd);
	while (GLenum e = glGetError()) {
		printf("OpenGL Error %d\n", e);
	}
	glBufferData(GL_DRAW_INDIRECT_BUFFER, sizeof(draw_cmd_t), &draw_cmd, GL_DYNAMIC_DRAW);
	while (GLenum e = glGetError()) {
		printf("OpenGL Error %d\n", e);
	}

	struct {
		float camera[16] = {};
		float view[16] = {};
		float proj[16] = {};
		float Fog[4] = {};
		float ExpFog[4] = {};
		float Density = 0;
	} CameraMatrix;

	GLuint cbs[3];
	glCreateBuffers(3, cbs);

	// First binding (General data)
	glBindBuffer(GL_UNIFORM_BUFFER, cbs[0]);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(CameraMatrix), &CameraMatrix, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, cbs[0]);
	while (GLenum e = glGetError()) {
		printf("OpenGL Error %d\n", e);
	}


	struct {
		float Model[16];
		float NormalMatrix[16];
	} Normals;

	// Second binding (Model and Normal data)
	glBindBuffer(GL_UNIFORM_BUFFER, cbs[1]);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(Normals), &Normals, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, cbs[1]);
	while (GLenum e = glGetError()) {
		printf("OpenGL Error %d\n", e);
	}

	struct {
		float globallightdir[4];
		float ambient[4];
		float Fog[4];
		float globallightcolor[4];
		float specularcolor[4];
		float camerapos[4];
	} WorldLight;

	// Third binding (Global light data)
	glBindBuffer(GL_UNIFORM_BUFFER, cbs[2]);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(WorldLight), &WorldLight, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 2, cbs[2]);
	while (GLenum e = glGetError()) {
		printf("OpenGL Error %d\n", e);
	}

	while (!glfwWindowShouldClose(win)) {
		glfwPollEvents();
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);



		//glDrawArrays(GL_TRIANGLES, 0, 3 * 4);
		vbuffer.Bind();
		while (GLenum e = glGetError()) {
			printf("OpenGL Error %d\n", e);
		}
		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, draw_buffer_cmd);
		while (GLenum e = glGetError()) {
			printf("OpenGL Error %d\n", e);
		}
		glDrawArraysIndirect(GL_TRIANGLES, 0);
		while (GLenum e = glGetError()) {
			printf("OpenGL Error %d\n", e);
		}


		glfwSwapBuffers(win);
	}

}
