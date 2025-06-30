#include "GLLibMain.hpp"
#include <GLFW/glfw3.h>
#include <GL/glew.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include <DirectXMath.h>

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

struct CameraMatrix {
	float camera[16] = {
	1.0f, 0.0, 0.0, 0.0,
	0.0, 1.0f, 0.0, 0.0,
	0.0, 0.0, 1.0f, 0.0,
	0.0, 0.0, 0.0, 1.0f,
	};
};

struct CameraData {
	GLuint buffer;
	float pos[3] = {};
	float pos_vec[3] = {};
	float rot[3] = {};
	CameraMatrix mt;
};

void CameraKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	CameraData* data = (CameraData*)glfwGetWindowUserPointer(window);
	if (action == GLFW_PRESS) {
		const float speed = 0.05f;
		switch (key) {
		case GLFW_KEY_W:
			data->pos_vec[2] = speed;
			break;
		case GLFW_KEY_A:
			data->pos_vec[0] = -speed;
			break;
		case GLFW_KEY_S:
			data->pos_vec[2] = -speed;
			break;
		case GLFW_KEY_D:
			data->pos_vec[0] = speed;
			break;
		case GLFW_KEY_SPACE:
			data->pos_vec[1] = speed;
			break;
		case GLFW_KEY_LEFT_CONTROL:
			data->pos_vec[1] = -speed;
			break;
		}

	}
	else if (action == GLFW_RELEASE) {
		switch (key) {
		case GLFW_KEY_W:
			data->pos_vec[2] = 0.0;
			break;
		case GLFW_KEY_A:
			data->pos_vec[0] = 0.0;
			break;
		case GLFW_KEY_S:
			data->pos_vec[2] = 0.0;
			break;
		case GLFW_KEY_D:
			data->pos_vec[0] = 0.0;
			break;
		case GLFW_KEY_SPACE:
			data->pos_vec[1] = 0.0;
			break;
		case GLFW_KEY_LEFT_CONTROL:
			data->pos_vec[1] = 0.0;
			break;
		}
	}
}

int main() {

	if (0 == glfwInit()) return -1;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	CameraData cmdata;

	GLFWwindow* win = glfwCreateWindow(512, 512, " ", nullptr, nullptr);
	if (nullptr == win) return -1;
	glfwMakeContextCurrent(win);

	if (GLEW_OK != glewInit()) return -1;

	size_t sz_vert = 0, sz_frag = 0;
	g_pVertSrc = (GLchar*)LoadTextFromFile("Transform3D.vert", &sz_vert);
	g_pFragSrc = (GLchar*)LoadTextFromFile("Transform3D.frag", &sz_frag);

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
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthRange(-1.0f, 1.0f);

	while (GLenum e = glGetError()) {
		printf("OpenGL Error %d\n", e);
	}

	Clb184::TLVertex3D verts[] = {
		{ -5.0f, 0.0f, 0.0f,   0xff0000ff,   0.0f, 0.0f,   0.0f, 1.0f, 0.0f },
		{ 5.0f, 0.0f, 0.0f,   0xff00ffff,   0.0f, 0.0f,   0.0f, 1.0f, 0.0f },
		{ -5.0f, 10.0f, 0.0f,   0xffff0000,   0.0f, 0.0f,   0.0f, 1.0f, 0.0f },
		{ 5.0f, 10.0f, 0.0f,   0xffffffff,   0.0f, 0.0f,   0.0f, 1.0f, 0.0f },
		{ 5.0f, 0.0f, 0.0f,   0xff00ffff,   0.0f, 0.0f,   0.0f, 1.0f, 0.0f },
		{ -5.0f, 10.0f, 0.0f,   0xffff0000,   0.0f, 0.0f,   0.0f, 1.0f, 0.0f },
	};

	GLuint vbuffer = -1;
	GLuint vattrib = -1;
	Clb184::CreateTL3DVertexBuffer(6, verts, GL_STATIC_DRAW, &vbuffer, &vattrib);

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


	Clb184::buffer_descriptor_t buf = { sizeof(draw_cmd_t), &draw_cmd, GL_DYNAMIC_DRAW};
	GLuint draw_buffer_cmd = -1;
	Clb184::CreateBuffer(&buf, &draw_buffer_cmd);

	glfwSetWindowUserPointer(win, &cmdata);
	glfwSetKeyCallback(win, CameraKeyCallback);



	// Normaldata
	struct Normals {
		float Model[16] = {
		1.0f, 0.0, 0.0, 0.0,
		0.0, 1.0f, 0.0, 0.0,
		0.0, 0.0, 1.0f, 0.0,
		0.0, 0.0, 0.0, 1.0f,
		};
	} normals;

	struct {
		float ambient[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	} WorldLight;

	Clb184::buffer_descriptor_t buf_desc[3] = {
		{sizeof(cmdata.mt), &cmdata.mt, GL_DYNAMIC_DRAW},
		{sizeof(normals), &normals, GL_DYNAMIC_DRAW},
		{sizeof(WorldLight), &WorldLight, GL_DYNAMIC_DRAW}
	};
	GLuint cbs[3] = { -1,-1,-1 };

	Clb184::CreateBuffers(buf_desc, cbs, 3);
	
	// First binding (General data)
	cmdata.buffer = cbs[0];
	cmdata.pos[0] = 0.0f;
	cmdata.pos[1] = -0.5f;
	cmdata.pos[2] = -10.0f;

	Clb184::BindConstantBuffer(cbs[0], 0);
	Clb184::BindConstantBuffer(cbs[1], 1);
	Clb184::BindConstantBuffer(cbs[2], 2);

	glfwSwapInterval(1);

	while (!glfwWindowShouldClose(win)) {
		glfwPollEvents();
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClearDepth(1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		cmdata.pos[0] += cmdata.pos_vec[0];
		cmdata.pos[1] += cmdata.pos_vec[1];
		cmdata.pos[2] += cmdata.pos_vec[2];


		CameraMatrix* pData = (CameraMatrix*)glMapNamedBuffer(cmdata.buffer, GL_WRITE_ONLY);

		DirectX::XMVECTOR eye_pos = { cmdata.pos[0], cmdata.pos[1], cmdata.pos[2], 1.0f};
		DirectX::XMMATRIX eye = DirectX::XMMatrixLookToLH(eye_pos, { 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f });
		//DirectX::XMMATRIX eye = DirectX::XMMatrixLookAtLH(eye_pos, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f });
		DirectX::XMMATRIX cam = DirectX::XMMatrixPerspectiveFovLH(3.14159f * 0.25f, 1.0f, 0.1f, 1000.0f);
		DirectX::XMMATRIX cam_eye = eye * cam;
		memcpy(&pData->camera, &cam_eye, sizeof(DirectX::XMMATRIX));
		glUnmapNamedBuffer(cmdata.buffer);
		//glDrawArrays(GL_TRIANGLES, 0, 3 * 4);
		glBindBuffer(GL_UNIFORM_BUFFER, cmdata.buffer);
		//vbuffer.Bind();
		glBindVertexArray(vattrib);
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

		//printf("x: %.3f, y: %.3f, z: %.3f\n", cmdata.pos[0], cmdata.pos[1], cmdata.pos[2]);
		glfwSwapBuffers(win);
	}

}
