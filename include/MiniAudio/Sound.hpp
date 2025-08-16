#ifndef SOUNDBUFFER_INCLUDED
#define SOUNDBUFFER_INCLUDED

#include <miniaudio.h>

namespace Clb184 {

	constexpr int AB_STOP = 0;
	constexpr int AB_PLAY = 1;

	struct audio_buffer_t {
		int status = false; // 0 -> STOP 1 -> PLAY
		ma_audio_buffer buffer_info;
	};

	struct sound_buffer_t {
		ma_int16* data; // PCM data
		int cnt = 0; // How many buffers
		audio_buffer_t* buffers = nullptr;
	};

	struct sound_buffer_container_t {
		int cnt = 0; // How many sound buffers
		sound_buffer_t* sound_buffers = nullptr;
	};

	struct sound_control_t {
		ma_device device;
		ma_decoder decoder;
		sound_buffer_container_t sounds;
	};

	// Initialize decoder and sound device
	bool InitializeSoundControl(sound_control_t* sound_control, int num_sound_buffers);

	// Set master volume
	void SetSoundMasterVolume(sound_control_t* sound_control, float level);

	// Create n buffers to play sound
	bool CreateSoundBuffer(ma_decoder* decoder, sound_buffer_t* sound_buffer, int cnt, const char* filename);
	void DestroySoundBuffer(sound_buffer_t* sound_buffer);

	// Play in normal mode or using x position as reference
	void Play(sound_control_t* sound_control, int index);
	void PlayX(sound_control_t* sound_control, int index, float x);

	// Move all sounds in sound buffers
	void SoundBufferPlayback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount);

}

#endif