// Framework and that stuff
#include "XPFW.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <immintrin.h>

// Includes for making DirectXMath work on Linux with no problems apparently
#ifdef __linux__
	#include <algorithm>
	#include <iterator>
	#include <utility>
	#include <format>
#endif

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
	int mov_bits = 0;
	int pad[2];
	float pos[4] = {};
	float rot[4] = {};
	CameraMatrix mt;
};

sound_control_t* snd_control = new sound_control_t;

void CameraKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	CameraData* data = (CameraData*)glfwGetWindowUserPointer(window);
	int mov_bits = data->mov_bits;
	if (action == GLFW_PRESS) {
		switch (key) {
			case GLFW_KEY_W: mov_bits |= 1; break;
			case GLFW_KEY_S: mov_bits |= 2; break;
			case GLFW_KEY_A: mov_bits |= 4; break;
			case GLFW_KEY_D: mov_bits |= 8; break;
			case GLFW_KEY_SPACE: mov_bits |= 16; break;
			case GLFW_KEY_LEFT_CONTROL: mov_bits |= 32; break;

			case GLFW_KEY_Z: PlaySndX(snd_control, 1, 0.0f); break;
			case GLFW_KEY_BACKSPACE: PlaySnd(snd_control, 0); break;

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

struct draw_cmd_t {
	GLuint count = 0;
	GLuint instance_cnt = 1;
	GLuint first = 0;
	GLuint base = 0;
};

struct TestData {
	// Camera data, obviously
	CameraData cmdata;

	// Shaders and their programs
	GLuint vrt, frg, prg;
	GLuint vrt2, frg2, prg2;

	// For Misaka texture and quad
	GLuint tex = 0xffffffff;
	int mw, mh;
	GLuint mvbo, mvao;

	GLuint vbuffer = -1, vattrib = -1;
	
	// Index data
	GLuint idxs[18]; 
	GLuint ibuffer = 0xffffffff;

	// Draw Indirect
	GLuint draw_buffer_cmd = 0xffffffff;

	// Constant buffers
	GLuint cbs[3] = { 0xffffffff, 0xffffffff, 0xffffffff };

	// Sampler
	GLuint sampler = 0xffffffff;

	// Font loading
	FT_Library library;
	font_descriptor_t font_desc;
	font_t* font = nullptr;

};

int InitializeAll(window_t* window, TestData* data) {
	GLERR;

	LoadShaderFromFile("Transform3D.vert", &data->vrt, GL_VERTEX_SHADER);
	LoadShaderFromFile("Transform3D.frag", &data->frg, GL_FRAGMENT_SHADER);
	if (false == CreateShaderProgram(data->vrt, data->frg, &data->prg)) { LOG_ERROR("Failed creating 3D shader"); return -1; }

	LoadShaderFromFile("T&L2D.vert", &data->vrt2, GL_VERTEX_SHADER);
	LoadShaderFromFile("T&L2D.frag", &data->frg2, GL_FRAGMENT_SHADER);
	if (false == CreateShaderProgram(data->vrt2, data->frg2, &data->prg2)) { LOG_ERROR("Failed creating 2D shader"); return -1; }

	//
	TLVertex2D mvert[4] = {
		{1280.0f, 720.0f - 90.0f, 1.0f, 0.0f, 0xffffffff},
		{1280.0f, 720.0f, 1.0f, 1.0f, 0xffffffff},
		{1280.0f - 160.0f, 720.0f - 90.0f, 0.0f, 0.0f, 0xffffffff},
		{1280.0f - 160.0f, 720.0f, 0.0f, 1.0f, 0xffffffff},
	};
	LoadTextureFromFile("misaka.png", &data->tex, &data->mw, &data->mh);
	CreateTL2DVertexBuffer(4, mvert, GL_STATIC_DRAW, &data->mvbo, &data->mvao);

	//
	TLVertex3D verts[] = {
		{ -5.0f, 0.0f, 0.0f,   0xff0000ff,   0.0f, 0.0f,   0.0f, 1.0f, 0.0f },     // 0
		{ 5.0f, 0.0f, 0.0f,   0xff00ffff,   0.0f, 0.0f,   0.0f, 1.0f, 0.0f },	   // 1
		{ -5.0f, 10.0f, 0.0f,   0xffff0000,   0.0f, 0.0f,   0.0f, 1.0f, 0.0f },	   // 2 
		{ 5.0f, 10.0f, 0.0f,   0xffffffff,   0.0f, 0.0f,   0.0f, 1.0f, 0.0f },	   // 3
		{ -5.0f, 10.0f, -10.0f,   0xffffffff,   0.0f, 0.0f,   0.0f, 1.0f, 0.0f },  // 4 
		{ -5.0f, 0.0f, -10.0f,   0xff00ffff,   0.0f, 0.0f,   0.0f, 1.0f, 0.0f },   // 5
		{ 5.0f, 10.0f, -10.0f,   0xffffffff,   0.0f, 0.0f,   0.0f, 1.0f, 0.0f },   // 6
		{ 5.0f, 0.0f, -10.0f,   0xff00ffff,   0.0f, 0.0f,   0.0f, 1.0f, 0.0f },	   // 7
	};

	CreateTL3DVertexBuffer(sizeof(verts) / sizeof(TLVertex3D), verts, GL_STATIC_DRAW, &data->vbuffer, &data->vattrib);

	//
	float identity[] = {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f,
	};
	DirectX::XMMATRIX prj = DirectX::XMMatrixOrthographicOffCenterLH(0.0, 1280.0, 720.0f, 0.0f, 1.0f, -1.0f);

	//
	glUseProgram(data->prg2);
	GL_ERROR();
	glUniformMatrix4fv(0, 1, GL_FALSE, (float*)&prj);
	GL_ERROR();

	glUseProgram(data->prg);
	GL_ERROR();
	draw_cmd_t draw_cmd; 
	draw_cmd.count = sizeof(mvert) / sizeof(TLVertex2D);

	buffer_descriptor_t buf = { sizeof(draw_cmd_t), &draw_cmd, GL_DYNAMIC_DRAW };
	CreateBuffer(&buf, &data->draw_buffer_cmd);


	// Indexes
	GLuint idxs[] = {0, 1, 2, 3, 1, 2, 0, 2, 4, 4, 0, 5, 4, 5, 6, 5, 6, 7};

	memcpy(data->idxs, idxs, sizeof(data->idxs));

	buffer_descriptor_t ibuffer_desc = { sizeof(idxs), idxs, GL_DYNAMIC_DRAW };
	CreateBuffer(&ibuffer_desc, &data->ibuffer);


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

	buffer_descriptor_t buf_desc[3] = {
		{sizeof(data->cmdata.mt), &data->cmdata.mt, GL_DYNAMIC_DRAW},
		{sizeof(normals), &normals, GL_DYNAMIC_DRAW},
		{sizeof(WorldLight), &WorldLight, GL_DYNAMIC_DRAW}
	};

	CreateBuffers(buf_desc, data->cbs, 3);

	// First binding (General data)
	data->cmdata.buffer = data->cbs[0];
	data->cmdata.pos[0] = 0.0f;
	data->cmdata.pos[1] = 0.0f;
	data->cmdata.pos[2] = -5.0f;

	BindConstantBuffer(data->cbs[0], 0);
	BindConstantBuffer(data->cbs[1], 1);
	BindConstantBuffer(data->cbs[2], 2);

	CreateSampler(&data->sampler);
	SetSamplerTextureMode(data->sampler, GL_NEAREST);
	SetSamplerWrapMode(data->sampler, GL_REPEAT, GL_REPEAT);
	glBindSampler(0, data->sampler);

	InitializeFreeType(&data->library);
	LoadFontFromFile(data->library, &data->font_desc, "PermanentMarker-Regular.ttf");
	data->font = new font_t;
	CreateFontWithAtlas(data->font_desc, data->font, 20.0f);

	glfwSetWindowUserPointer(window->window, &data->cmdata);
	glfwSetKeyCallback(window->window, CameraKeyCallback);
	return 0;
}

void DummyLoop(float delta_time, void* data) {
	TestData* dat = (TestData*)data; 
	GLERR;
	
	// Move logic and update stuff
	glUseProgram(dat->prg);
	MoveCamera(&dat->cmdata, dat->cmdata.mov_bits, delta_time);
	glBindBuffer(GL_UNIFORM_BUFFER, dat->cmdata.buffer);
	glBindVertexArray(dat->vattrib);
	GL_ERROR();


	// Draw with command buffer or other related functions
	glEnable(GL_DEPTH_TEST);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, dat->ibuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDrawElements(GL_TRIANGLES, sizeof(dat->idxs) / sizeof(GLuint), GL_UNSIGNED_INT, 0);
	GL_ERROR();

	// Draw 2D
	glDisable(GL_DEPTH_TEST);
	glActiveTexture(GL_TEXTURE0);
	glUseProgram(dat->prg2);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, dat->draw_buffer_cmd);
	GL_ERROR();

	// Draw font atlas
	glBindVertexArray(dat->mvao);
	glBindTexture(GL_TEXTURE_2D, dat->tex);
	glDrawArraysIndirect(GL_TRIANGLE_STRIP, 0);
	GL_ERROR();
	DrawString(dat->font, 0.0f, 0.0f,
		"Finally, I compiled this thing with LLVM! Isn't that crazy?\n"
		"There's still some other stuff I should try as well, me thinks"
	);
	char bf[24] = "";
	sprintf(bf, "%.2f fps", 1.0f / delta_time);
	DrawString(dat->font, 0.0f, 640.0f, bf);
}

int main() {
	LOG_INFO("Initializing demo");
	InitializeSoundControl(snd_control, 3);

	// We only play Vorbis files now
	CreateSoundBuffer(snd_control, 0, 1, "exboss_2.ogg");
	CreateSoundBuffer(snd_control, 1, 4, "ChargeSE.ogg");

	// Create window
	window_t window_data;
	TestData data;

	CreateGLWindow("OpenGL 4.6", 1280, 720, false, DummyLoop, &data, &window_data);
	InitializeAll(&window_data, &data);
	RunMainLoop(&window_data);
	DestroyGLWindow(&window_data);

	DestroySoundControl(snd_control);
}
