#ifndef SOUNDBUFFER_INCLUDED
#define SOUNDBUFFER_INCLUDED

#include <miniaudio.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

const int AB_STOP = 0;
const int AB_PLAY = 1;

typedef struct {
	int status; // = false; // 0 -> STOP 1 -> PLAY
	float place; // = 0.0f;
	ma_audio_buffer buffer_info;
} audio_buffer_t;

typedef struct {
	int channels; // = 0;
	ma_int16* data; // PCM data
	int cnt; // = 0; // How many buffers
	audio_buffer_t* buffers; // = nullptr;
} sound_buffer_t;

typedef struct  {
	int cnt; // = 0; // How many sound buffers
	sound_buffer_t* sound_buffers; // = nullptr;
} sound_buffer_container_t;

typedef struct  {
	ma_device device;
	sound_buffer_container_t sounds;
} sound_control_t;

// Initialize decoder and sound device
bool InitializeSoundControl(sound_control_t* sound_control, int num_sound_buffers);
void DestroySoundControl(sound_control_t* sound_control);

// Set master volume
void SetSoundMasterVolume(sound_control_t* sound_control, float level);

// Create n buffers to play sound
bool CreateSoundBuffer(sound_control_t* sound_control, int index, int cnt, const char* filename);
void DestroySoundBuffer(sound_control_t* sound_control, int index);

// Play in normal mode or using x position as reference
void PlaySnd(sound_control_t* sound_control, int index);
void PlaySndX(sound_control_t* sound_control, int index, float x);

// Move all sounds in sound buffers
void SoundBufferPlayback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount);

#ifdef __cplusplus
}
#endif

#endif