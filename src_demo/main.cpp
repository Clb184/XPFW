#include "GLLibMain.hpp"
#include <GLFW/glfw3.h>
#include <GL/glew.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <immintrin.h>

#ifdef __linux__
	#include <algorithm>
	#include <iterator>
	#include <utility>
	#include <format>
#endif
#include <DirectXMath.h>
#include <miniaudio.h>
#include <ft2build.h>
#include FT_FREETYPE_H

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
	int mov_bits = 0;
	int pad[2];
	float pos[4] = {};
	float rot[4] = {};
	CameraMatrix mt;
};

#include "MiniAudio/Sound.hpp"

Clb184::sound_control_t* snd_control = new Clb184::sound_control_t;

void CameraKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	CameraData* data = (CameraData*)glfwGetWindowUserPointer(window);
	int mov_bits = data->mov_bits;
	if (action == GLFW_PRESS) {
		//Clb184::PlaySnd(snd_control, 2);
		switch (key) {
			case GLFW_KEY_W: mov_bits |= 1; break;
			case GLFW_KEY_S: mov_bits |= 2; break;
			case GLFW_KEY_A: mov_bits |= 4; break;
			case GLFW_KEY_D: mov_bits |= 8; break;
			case GLFW_KEY_SPACE: mov_bits |= 16; break;
			case GLFW_KEY_LEFT_CONTROL: mov_bits |= 32; break;

			case GLFW_KEY_Z: Clb184::PlaySndX(snd_control, 1, 400.0f); break;

			case GLFW_KEY_LEFT: mov_bits |= 256; break;
			case GLFW_KEY_RIGHT: mov_bits |= 512; break;
			case GLFW_KEY_UP: mov_bits |= 1024; break;
			case GLFW_KEY_DOWN: mov_bits |= 2048; break;
		}
	}
	if (action == GLFW_RELEASE) {
		switch (key) {
			case GLFW_KEY_W: mov_bits &= ~1; break;
			case GLFW_KEY_S: mov_bits &= ~2; break;
			case GLFW_KEY_A: mov_bits &= ~4; break;
			case GLFW_KEY_D: mov_bits &= ~8; break;
			case GLFW_KEY_SPACE: mov_bits &= ~16; break;
			case GLFW_KEY_LEFT_CONTROL: mov_bits &= ~32; break;

			case GLFW_KEY_LEFT: mov_bits &= ~256; break;
			case GLFW_KEY_RIGHT: mov_bits &= ~512; break;
			case GLFW_KEY_UP: mov_bits &= ~1024; break;
			case GLFW_KEY_DOWN: mov_bits &= ~2048; break;
		}
	}

	data->mov_bits = mov_bits;
}

void MoveCamera(CameraData* camera_data, int mov_bits, float delta_time) {
	const float speed = 20.0f * delta_time;

	const float rot_vel = 3.14159 * 1.0f * delta_time;
	float rot_x = 0.0f, rot_y = 0.0f;
	// Rotation
	if (mov_bits & 256) rot_y -= rot_vel; // Y -
	if (mov_bits & 512) rot_y += rot_vel; // Y +
	if (mov_bits & 1024) rot_x -= rot_vel; // X +
	if (mov_bits & 2048) rot_x += rot_vel; // X 
	camera_data->rot[0] += rot_x;
	camera_data->rot[1] += rot_y;
	float mov_x = cosf(camera_data->rot[1]), mov_y = sinf(camera_data->rot[1]);

	// Position move vector
	float spd_front = 0.0f, spd_side = 0.0f;
	if (mov_bits & 1)  spd_front += speed; // Z +
	if (mov_bits & 2)  spd_front -= speed; // Z -
	if (mov_bits & 4)  spd_side -= speed; // X -
	if (mov_bits & 8)  spd_side += speed; // X +
	if (mov_bits & 16) camera_data->pos[1] += speed; // Y +
	if (mov_bits & 32) camera_data->pos[1] -= speed; // Y -+
	camera_data->pos[0] += mov_y * spd_front + mov_x * spd_side;
	camera_data->pos[2] += mov_x * spd_front - mov_y * spd_side;

	// Rotate using quaternions
	__m128 rot = _mm_mul_ps(_mm_load_ps(camera_data->rot), _mm_set_ps1(0.5f)); // Load and get the half already to use it with quaternions
	__m128 sins = DirectX::XMVectorSin(rot);// = _mm_sin_ps(rot); //Calculate sin of all axis
	__m128 coss = DirectX::XMVectorCos(rot);// = _mm_cos_ps(rot); //Same for cos
	
	DirectX::XMFLOAT3 rts, rtc; //Sine and cosine Obtained
	DirectX::XMStoreFloat3(&rts, sins);
	DirectX::XMStoreFloat3(&rtc, coss);
	DirectX::XMVECTOR q = DirectX::XMQuaternionMultiply(_mm_set_ps(rtc.x, 0.0f, 0.0f, rts.x), _mm_set_ps(rtc.y, 0.0f, rts.y, 0.0f)); // Get the rotation of camera
	DirectX::XMVECTOR o = (DirectX::XMVector4Transform(_mm_set_ps(1.0f, 1.0f, 0.0f, 0.0f), DirectX::XMMatrixRotationQuaternion(q)));
	
	// Update matrix
	DirectX::XMVECTOR eye_pos = _mm_load_ps(camera_data->pos);
	DirectX::XMMATRIX eye = DirectX::XMMatrixLookToLH(eye_pos, o, _mm_set_ps(0.0, 0.0f, 1.0f, 0.0f));
	DirectX::XMMATRIX cam = DirectX::XMMatrixPerspectiveFovLH(3.14159f * 0.25f, 16.0f/9.0f, 0.1f, 1000.0f);
	DirectX::XMMATRIX cam_eye = eye * cam;

	// Write and send data to the GPU
	CameraMatrix* pData = (CameraMatrix*)glMapNamedBuffer(camera_data->buffer, GL_WRITE_ONLY);
	memcpy(&pData->camera, &cam_eye, sizeof(DirectX::XMMATRIX));
	glUnmapNamedBuffer(camera_data->buffer);
}

struct audio_buffer_t {
	int cnt = 0;
	ma_audio_buffer buffers[64];
};

void DataPlayback (ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) {
	audio_buffer_t* abuffer = (audio_buffer_t*)pDevice->pUserData;
	ma_int16* buf = (ma_int16*)pOutput;
	ma_int16 buff[4096] = {0};
	if (nullptr == abuffer) return;
	for (int i = 0; i < abuffer->cnt; i++ ) {
		ma_uint64 frms = ma_audio_buffer_read_pcm_frames(abuffer->buffers + i, buff, frameCount, false);
		for (ma_uint64 j = 0; j < frms * 2; j++) {
			int max = buf[j] + buff[j];
			buf[j] = (max > INT16_MAX) ? INT16_MAX : (max < INT16_MIN) ? INT16_MIN : max;
		}
	}
}

void CreateSoundBuffer(ma_decoder* decoder, ma_uint64 frames, int channel, ma_int16** data) {
	ma_int16* samples = new ma_int16[frames * channel + 2];
	ma_int16* offset = samples;
	ma_uint64 frm = 0;
	ma_uint64 frm_total = 0;
	do {
		ma_decoder_read_pcm_frames(decoder, offset, 4096, &frm);
		offset += frm * channel;
		frm_total += frm;
	} while (frm);
	*data = samples;
	return;
}


int main() {
	Clb184::InitializeSoundControl(snd_control, 3);
	Clb184::CreateSoundBuffer(snd_control, 0, 1, "exboss_2.wav");
	Clb184::CreateSoundBuffer(snd_control, 1, 4, "ChargeSE.wav");
	Clb184::CreateSoundBuffer(snd_control, 2, 4, "Typing.wav");
	//Clb184::PlaySndX(snd_control, 0, 200.0f);

	// Back with OpenGL...
	if (0 == glfwInit()) return -1;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	//glfwWindowHint(GLFW_SAMPLES, 16);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	CameraData cmdata;

	GLFWwindow* win = glfwCreateWindow(1280, 720, "OpenGL 4.6", nullptr, nullptr);
	if (nullptr == win) return -1;
	glfwMakeContextCurrent(win);
#ifdef WIN32
	if (GLEW_OK != glewInit()) { LOG_ERROR("Failed initializing GLEW"); return -1; }
#elif defined linux
	glewInit();
#endif

	// Render buffer test
	// Textures on OpenGL are flipped...
	// Can't do much about it, but that's how it works
	GLuint framebuff, render_tex;
	glCreateTextures(GL_TEXTURE_2D, 1, &render_tex);
	glBindTexture(GL_TEXTURE_2D, render_tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1280, 720, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glBindTexture(GL_TEXTURE_2D, 0);

	glCreateFramebuffers(1, &framebuff);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuff);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, render_tex, 0);
	if (GL_FRAMEBUFFER_COMPLETE != glCheckFramebufferStatus(GL_FRAMEBUFFER)) {
		printf("Framebuffer incomplete\n");
		return -1;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	Clb184::TLVertex2D fvert[4] = {
		{1280.0f, 0.0f, 1.0f, 1.0f, 0xffffffff},
		{1280.0f, 720.0f, 1.0f, 0.0f, 0xffffffff},
		{0.0f, 0.0f, 0.0f, 1.0f, 0xffffffff},
		{0.0f, 720.0f, 0.0f, 0.0f, 0xffffffff},
	};
	GLuint fvbo, fvao;
	Clb184::CreateTL2DVertexBuffer(4, fvert, GL_STATIC_DRAW, &fvbo, &fvao);

	// Configuration of scene
	//glEnable(GL_MULTISAMPLE);
	glDisable(GL_CULL_FACE);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthRange(-1.0f, 1.0f);

	while (GLenum e = glGetError()) {
		printf("OpenGL Error %d\n", e);
	}

	GLuint vrt, frg, prg;
	Clb184::LoadShaderFromFile("Transform3D.vert", &vrt, GL_VERTEX_SHADER);
	Clb184::LoadShaderFromFile("Transform3D.frag", &frg, GL_FRAGMENT_SHADER);
	if (false == Clb184::CreateProgram(vrt, frg, &prg)) { LOG_ERROR("Failed creating 3D shader"); return -1; }

	GLuint vrt2, frg2, prg2;
	Clb184::LoadShaderFromFile("T&L2D.vert", &vrt2, GL_VERTEX_SHADER);
	Clb184::LoadShaderFromFile("T&L2D.frag", &frg2, GL_FRAGMENT_SHADER);
	if (false == Clb184::CreateProgram(vrt2, frg2, &prg2)) { LOG_ERROR("Failed creating 2D shader"); return -1; }


	GLuint tex = -1;
	Clb184::TLVertex2D mvert[4] = {
		{512.0f, 0.0f, 1.0f, 0.0f, 0xffffffff},
		{512.0f, 512.0f, 1.0f, 1.0f, 0xffffffff},
		{0.0f, 0.0f, 0.0f, 0.0f, 0xffffffff},
		{0.0f, 512.0f, 0.0f, 1.0f, 0xffffffff},
	};
	int mw, mh;
	Clb184::LoadTextureFromFile("mikoto.png", &tex, &mw, &mh);
	GLuint mvbo, mvao;
	Clb184::CreateTL2DVertexBuffer(4, mvert, GL_STATIC_DRAW, &mvbo, &mvao);

	Clb184::TLVertex3D verts[] = {
		{ -5.0f, 0.0f, 0.0f,   0xff0000ff,   0.0f, 0.0f,   0.0f, 1.0f, 0.0f },     // 0
		{ 5.0f, 0.0f, 0.0f,   0xff00ffff,   0.0f, 0.0f,   0.0f, 1.0f, 0.0f },	   // 1
		{ -5.0f, 10.0f, 0.0f,   0xffff0000,   0.0f, 0.0f,   0.0f, 1.0f, 0.0f },	   // 2 
		{ 5.0f, 10.0f, 0.0f,   0xffffffff,   0.0f, 0.0f,   0.0f, 1.0f, 0.0f },	   // 3
		{ -5.0f, 10.0f, -10.0f,   0xffffffff,   0.0f, 0.0f,   0.0f, 1.0f, 0.0f },  // 4 
		{ -5.0f, 0.0f, -10.0f,   0xff00ffff,   0.0f, 0.0f,   0.0f, 1.0f, 0.0f },   // 5
		{ 5.0f, 10.0f, -10.0f,   0xffffffff,   0.0f, 0.0f,   0.0f, 1.0f, 0.0f },   // 6
		{ 5.0f, 0.0f, -10.0f,   0xff00ffff,   0.0f, 0.0f,   0.0f, 1.0f, 0.0f },	   // 7
	};

	GLuint vbuffer = -1;
	GLuint vattrib = -1;
	Clb184::CreateTL3DVertexBuffer(sizeof(verts) / sizeof(Clb184::TLVertex3D), verts, GL_STATIC_DRAW, &vbuffer, &vattrib);

	GLuint idxs[] = { 0, 1, 2, 3, 1, 2, 0, 2, 4, 4, 0, 5, 4, 5, 6, 5, 6, 7};

	Clb184::buffer_descriptor_t ibuffer_desc = { sizeof(idxs), idxs, GL_DYNAMIC_DRAW };
	GLuint ibuffer = -1;
	Clb184::CreateBuffer(&ibuffer_desc, &ibuffer);

	float identity[] = {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f,
	};
	DirectX::XMMATRIX prj = DirectX::XMMatrixOrthographicOffCenterLH(0.0, 1280.0, 720.0f, 0.0f, 1.0f, -1.0f);

	glUseProgram(prg2);
	GL_ERROR();
	glUniformMatrix4fv(0, 1, GL_FALSE, (float*)&prj);
	GL_ERROR();

	glUseProgram(prg);
	GL_ERROR();

	struct draw_cmd_t {
		GLuint count = sizeof(mvert) / sizeof(Clb184::TLVertex2D);
		GLuint instance_cnt = 1;
		GLuint first = 0;
		GLuint base = 0;
	} draw_cmd;

	Clb184::buffer_descriptor_t buf = { sizeof(draw_cmd_t), &draw_cmd, GL_DYNAMIC_DRAW };
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
	GLuint cbs[3] = { 0xffffffff, 0xffffffff, 0xffffffff };

	Clb184::CreateBuffers(buf_desc, cbs, 3);

	// First binding (General data)
	cmdata.buffer = cbs[0];
	cmdata.pos[0] = 0.0f;
	cmdata.pos[1] = 0.0f;
	cmdata.pos[2] = -5.0f;

	Clb184::BindConstantBuffer(cbs[0], 0);
	Clb184::BindConstantBuffer(cbs[1], 1);
	Clb184::BindConstantBuffer(cbs[2], 2);

	glfwSwapInterval(1); // Actually vsync
	double delta_time = 0.0;

	GLuint sampler;
	Clb184::CreateSampler(&sampler);
	Clb184::SetSamplerTextureMode(sampler, GL_NEAREST);
	Clb184::SetSamplerWrapMode(sampler, GL_REPEAT, GL_REPEAT);
	glBindSampler(0, sampler);

	FT_Library library;
	Clb184::font_descriptor_t font_desc;
	Clb184::font_t* font = new Clb184::font_t;
	Clb184::InitializeFreeType(&library);
	Clb184::LoadFontFromFile(library, &font_desc, "PermanentMarker-Regular.ttf");
	Clb184::CreateFontWithAtlas(font_desc, font, 20.0f);

	while (!glfwWindowShouldClose(win)) {
		delta_time = glfwGetTime();
		const float aver = 1.0f / delta_time;
		//printf("delta_time: %5.4f\n", aver);
		glfwSetTime(0.0);

		// Process events and clear screen
		glfwPollEvents();
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClearDepth(1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Move logic and update stuff
		glUseProgram(prg);
		MoveCamera(&cmdata, cmdata.mov_bits, delta_time);
		glBindBuffer(GL_UNIFORM_BUFFER, cmdata.buffer);
		glBindVertexArray(vattrib);
		GL_ERROR();


		// Draw with command buffer or other related functions
		glEnable(GL_DEPTH_TEST);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibuffer);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDrawElements(GL_TRIANGLES, sizeof(idxs) / sizeof(GLuint), GL_UNSIGNED_INT, 0);
		GL_ERROR();

		// Draw 2D
		glDisable(GL_DEPTH_TEST);
		glActiveTexture(GL_TEXTURE0);
		glUseProgram(prg2);
		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, draw_buffer_cmd);
		GL_ERROR();
		// Draw RenderTex
		glBindTexture(GL_TEXTURE_2D, render_tex);
		glBindVertexArray(fvao);
		//glDrawArraysIndirect(GL_TRIANGLE_STRIP, 0);
		GL_ERROR();
		// Draw font atlas
		glBindVertexArray(mvao);
		//glDrawArraysIndirect(GL_TRIANGLE_STRIP, 0);
		GL_ERROR();
		Clb184::DrawString(font, 0.0f, 0.0f,
			"Hello World!, I'm doing fine, And YOU?\n"
			"Well, I'm just trying to test if I can get away with writing whatever text I want\n"
			"Will this RTX 3050 6GB stand this? with OpenGL 4.6\n"
			"I should try this on my other machine with an AMD A4-6210 with Radeon R3 graphics as well"
		);
		char bf[10] = "";
		sprintf(bf, "%.2f", aver);
		Clb184::DrawString(font, 0.0f, 640.0f, bf);
		// Move the Swap Chain
		glfwSwapBuffers(win);
	}

	//ma_device_uninit(&audio_device);
	//ma_decoder_uninit(&decoder);
}
