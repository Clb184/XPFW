#include "GLLibMain.hpp"
#include <GLFW/glfw3.h>
#include <GL/glew.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include <DirectXMath.h>

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
	int mov_bits = 0;
	CameraMatrix mt;
};

void CameraKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	CameraData* data = (CameraData*)glfwGetWindowUserPointer(window);
	const float speed = 0.05f;
	int mov_bits = data->mov_bits;
	if (action == GLFW_PRESS) {
		switch (key) {
			case GLFW_KEY_W: mov_bits |= 1; break;
			case GLFW_KEY_A: mov_bits |= 4; break;
			case GLFW_KEY_S: mov_bits |= 2; break;
			case GLFW_KEY_D: mov_bits |= 8; break;
			case GLFW_KEY_SPACE: mov_bits |= 16; break;
			case GLFW_KEY_LEFT_CONTROL: mov_bits |= 32; break;
		}
	}
	if (action == GLFW_RELEASE) {
		switch (key) {
			case GLFW_KEY_W: mov_bits ^= 1; break;
			case GLFW_KEY_A: mov_bits ^= 4; break;
			case GLFW_KEY_S: mov_bits ^= 2; break;
			case GLFW_KEY_D: mov_bits ^= 8; break;
			case GLFW_KEY_SPACE: mov_bits ^= 16; break;
			case GLFW_KEY_LEFT_CONTROL: mov_bits ^= 32; break;
		}
	}
	float mov_vec[3] = { 0.0f };
	// Position move vector
	if (mov_bits & 1) mov_vec[2] += speed; // Z +
	if (mov_bits & 2) mov_vec[2] -= speed; // Z -
	if (mov_bits & 4) mov_vec[0] -= speed; // X -
	if (mov_bits & 8) mov_vec[0] += speed; // X +
	if (mov_bits & 16) mov_vec[1] += speed; // Y +
	if (mov_bits & 32) mov_vec[1] -= speed; // Y -

	data->pos_vec[0] = mov_vec[0];
	data->pos_vec[1] = mov_vec[1];
	data->pos_vec[2] = mov_vec[2];
	data->mov_bits = mov_bits;
}

int main() {

	if (0 == glfwInit()) return -1;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	CameraData cmdata;

	GLFWwindow* win = glfwCreateWindow(512, 512, "OpenGL 4.6", nullptr, nullptr);
	if (nullptr == win) return -1;
	glfwMakeContextCurrent(win);

	if (GLEW_OK != glewInit()) return -1;

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

	GLuint vrt, frg, prg;
	Clb184::LoadShaderFromFile("Transform3D.vert", &vrt, GL_VERTEX_SHADER);
	Clb184::LoadShaderFromFile("Transform3D.frag", &frg, GL_FRAGMENT_SHADER);

	if(false == Clb184::CreateProgram(vrt, frg, &prg)) return -1;


	Clb184::TLVertex3D verts[] = {
		{ -5.0f, 0.0f, 0.0f,   0xff0000ff,   0.0f, 0.0f,   0.0f, 1.0f, 0.0f },
		{ 5.0f, 0.0f, 0.0f,   0xff00ffff,   0.0f, 0.0f,   0.0f, 1.0f, 0.0f },
		{ -5.0f, 10.0f, 0.0f,   0xffff0000,   0.0f, 0.0f,   0.0f, 1.0f, 0.0f },
		{ 5.0f, 10.0f, 0.0f,   0xffffffff,   0.0f, 0.0f,   0.0f, 1.0f, 0.0f },
	};

	GLuint vbuffer = -1;
	GLuint vattrib = -1;
	Clb184::CreateTL3DVertexBuffer(6, verts, GL_STATIC_DRAW, &vbuffer, &vattrib);

	GLuint idxs[] = {0, 1, 2, 3, 1, 2};

	Clb184::buffer_descriptor_t ibuffer_desc = {sizeof(GLuint) * 6, idxs, GL_DYNAMIC_DRAW};
	GLuint ibuffer = -1;
	Clb184::CreateBuffer(&ibuffer_desc, &ibuffer);

	float identity[] = {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f,
	};
	glUseProgram(prg);
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
		glBindVertexArray(vattrib);
		while (GLenum e = glGetError()) {
			printf("OpenGL Error %d\n", e);
		}
		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, draw_buffer_cmd);
		while (GLenum e = glGetError()) {
			printf("OpenGL Error %d\n", e);
		}
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibuffer);
		//glDrawArraysIndirect(GL_TRIANGLES, 0);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		while (GLenum e = glGetError()) {
			printf("OpenGL Error %d\n", e);
		}
		glfwSwapBuffers(win);
	}

}
