#include <MiniAudio/Sound.h>
#include <assert.h>
#include "MiniAudio/OGGDecode.h"
#include "Output.h"

bool InitializeSoundControl(sound_control_t* sound_control, int num_sound_buffers) {
	LOG_INFO("Initializing Sound Control");
	assert(0 != sound_control);

	ma_device_config cfg;
	cfg = ma_device_config_init(ma_device_type_playback);
	cfg.playback.format = ma_format_s16;
	cfg.playback.channels = 2;
	cfg.sampleRate = 44100;
	cfg.dataCallback = SoundBufferPlayback;
	cfg.pUserData = sound_control;
	sound_control->device;

	// Initialize device
	if (MA_SUCCESS != ma_device_init(0, &cfg, &sound_control->device)) {
		return false;
	}

	// Allocate all sound buffers needed
	sound_control->sounds.cnt = num_sound_buffers;
	if(0 == num_sound_buffers) {
		sound_control->sounds.sound_buffers = calloc(1, sizeof(sound_buffer_t));
		if (0 == sound_control->sounds.sound_buffers) return false;
	}
	else {
		sound_control->sounds.sound_buffers = calloc(num_sound_buffers, sizeof(sound_buffer_t));
		if (0 == sound_control->sounds.sound_buffers) return false;
	}

	// Initialize Music stream
	sound_control->music.status = MSS_STOP;
	sound_control->music.looped = 0;
	sound_control->music.cursor = 0;
	sound_control->music.start_loop = 0;
	sound_control->music.end_loop = 0;
	sound_control->music.num_samples = 0;
	sound_control->music.num_channels = 0;
	sound_control->music.samples = 0;

	// Now start sound
	if (MA_SUCCESS != ma_device_start(&sound_control->device)) {
		ma_device_uninit(&sound_control->device);
		return false;
	}

	return true;
}

void DestroySoundControl(sound_control_t* sound_control) {
	LOG_INFO("Destroying Sound Control");
	assert(0 != sound_control);
	ma_device_uninit(&sound_control->device);
	
	// Destroy each sound buffer instance
	for(int i = 0; i < sound_control->sounds.cnt; i++) {
		DestroySoundBuffer(sound_control, i);
	}

	// Free all
	if(0 != sound_control->sounds.sound_buffers) {
		free(sound_control->sounds.sound_buffers);
		sound_control->sounds.sound_buffers = 0;
	}

	// Free music data
	if(0 != sound_control->music.samples) {
		free(sound_control->music.samples);
		sound_control->music.samples = 0;
	}
}

void SetSoundMasterVolume(sound_control_t* sound_control, float level) {
	assert(0 != sound_control);
	ma_device_set_master_volume(&sound_control->device, level);
}

bool LoadSoundFromFile(sound_control_t* sound_control, int index, int cnt, const char* filename) {
	char buf[256] = "";
	sprintf(buf, "Creating Sound Buffer (%d) for \"%s\" x%d", index, filename, cnt);
	LOG_INFO(buf);
	assert(0 != sound_control);
	assert(sound_control->sounds.cnt > index);

	ma_result res;
	ma_audio_buffer_config cfg;

	// Load vorbis data and copy to a buffer we'll use
	vorbis_data_t vorbis;
	if (false == LoadVorbisFile(filename, &vorbis)) return false;
	cfg = ma_audio_buffer_config_init(ma_format_s16, vorbis.channels, vorbis.sample_count, vorbis.sample_data, 0);

	sound_buffer_t* sound_buffer = &sound_control->sounds.sound_buffers[index];

	sound_buffer->cnt = cnt;
	sound_buffer->data = vorbis.sample_data;
	sound_buffer->buffers = calloc(cnt, sizeof(audio_buffer_t));
	sound_buffer->channels = vorbis.channels;

	if (0 == sound_buffer->buffers) return false;

	for (int i = 0; i < cnt; i++) {
		res = ma_audio_buffer_init(&cfg, &sound_buffer->buffers[i].buffer_info);
	}
}

bool LoadSoundFromMemory(sound_control_t* sound_control, int index, int cnt, char* data, size_t size) {
	char buf[256] = "";
	sprintf(buf, "Creating Sound Buffer (%d) from memory data x%d", index, cnt);
	LOG_INFO(buf);
	assert(0 != sound_control);
	assert(sound_control->sounds.cnt > index);

	ma_result res;
	ma_audio_buffer_config cfg;

	// Load vorbis data and copy to a buffer we'll use
	vorbis_data_t vorbis;
	if (false == LoadVorbisFromMemory(data, size, &vorbis)) return false;
	cfg = ma_audio_buffer_config_init(ma_format_s16, vorbis.channels, vorbis.sample_count, vorbis.sample_data, 0);

	sound_buffer_t* sound_buffer = &sound_control->sounds.sound_buffers[index];

	sound_buffer->cnt = cnt;
	sound_buffer->data = vorbis.sample_data;
	sound_buffer->buffers = calloc(cnt, sizeof(audio_buffer_t));
	sound_buffer->channels = vorbis.channels;

	if (0 == sound_buffer->buffers) return false;

	for (int i = 0; i < cnt; i++) {
		res = ma_audio_buffer_init(&cfg, &sound_buffer->buffers[i].buffer_info);
	}
}

void DestroySoundBuffer(sound_control_t* sound_control, int index) {
	LOG_INFO("Destroying Sound Buffer");
	assert(0 != sound_control);
	assert(0 != sound_control->sounds.sound_buffers);
	assert(sound_control->sounds.cnt > index);

	// Proceeed with the sound buffer
	sound_buffer_t* sound_buffer = &sound_control->sounds.sound_buffers[index];

	sound_buffer->cnt = 0; // Zero available buffers

	if(0 != sound_buffer->buffers) {
		free(sound_buffer->buffers); // Delete all buffers
		sound_buffer->buffers = 0;
	}

	if(0 != sound_buffer->data) {
		free(sound_buffer->data); // Delete the PCM data as is not needed anymore
		sound_buffer->data = 0;
	}
}

void PlaySnd(sound_control_t* sound_control, int index) {
	const int cnt = sound_control->sounds.sound_buffers[index].cnt;
	audio_buffer_t* buffer = sound_control->sounds.sound_buffers[index].buffers;
	for (int i = 0; i < cnt; i++) {
		if (buffer[i].status == AB_PLAY) continue;
		buffer[i].status = AB_PLAY;
		buffer[i].place = 0.0f;
		ma_audio_buffer_seek_to_pcm_frame(&buffer[i].buffer_info, 0);
		break;
	}
}

const float div_place = 1.0f / 300.0f;
void PlaySndX(sound_control_t* sound_control, int index, float x) {
	const int cnt = sound_control->sounds.sound_buffers[index].cnt;
	audio_buffer_t* buffer = sound_control->sounds.sound_buffers[index].buffers;
	for (int i = 0; i < cnt; i++) {
		if (buffer[i].status == AB_PLAY) continue;
		buffer[i].status = AB_PLAY;
		const float place = x * div_place;
		buffer[i].place = (1.0f < place) ? 1.0f : (place < -1.0f) ? -1.0f : place;
		ma_audio_buffer_seek_to_pcm_frame(&buffer[i].buffer_info, 0);
		break;
	}
}


int LoadMusicFromFile(sound_control_t* sound_control, const char* filename) {
	char buf[256];
	sprintf(buf, "Loading music file \"%s\"", filename);
	LOG_INFO(buf);
	assert(0 != sound_control);

	ma_result res;
	ma_audio_buffer_config cfg;

	// Load vorbis data and copy to a buffer we'll use
	vorbis_data_t vorbis;
	if (false == LoadVorbisFile(filename, &vorbis)) {
		LOG_ERROR("Failed loading music file");
		return -1;
	}

	sound_control->music.status = MSS_STOP;
	sound_control->music.start_loop = 0;
	sound_control->music.end_loop = 0;
	sound_control->music.num_samples = vorbis.sample_count * vorbis.channels;
	if(0 != sound_control->music.samples) {
		free(sound_control->music.samples);
	}
	sound_control->music.samples = vorbis.sample_data;
	sound_control->music.num_channels = vorbis.channels;
	sound_control->music.cursor = 0;
	return 0;
}

int LoadMusicFromMemory(sound_control_t* sound_control, char* data, size_t size){
	LOG_INFO("Loading music from Memory");
	assert(0 != sound_control);

	ma_result res;
	ma_audio_buffer_config cfg;

	// Load vorbis data and copy to a buffer we'll use
	vorbis_data_t vorbis;
	if (false == LoadVorbisFromMemory(data, size, &vorbis)) {
		LOG_ERROR("Failed loading music data");
		return -1;
	}

	sound_control->music.status = MSS_STOP;
	sound_control->music.start_loop = 0;
	sound_control->music.end_loop = 0;
	sound_control->music.num_samples = vorbis.sample_count; // * vorbis.channels;
	if(0 != sound_control->music.samples) {
		free(sound_control->music.samples);
	}
	sound_control->music.samples = vorbis.sample_data;
	sound_control->music.num_channels = vorbis.channels;
	sound_control->music.cursor = 0;
	return 0;
}

void MusicSetLoop(sound_control_t* sound_control, uint64_t start, uint64_t end) {
	
	// Clamp to music area
	if(sound_control->music.num_samples < start) {
		start = sound_control->music.num_samples;
	}
	if (sound_control->music.num_samples < end) {
		end = sound_control->music.num_samples;
	}

	// If loop start is bigger than end or end is less or equal than zero, make it the end of the loop
	if(end < start || 0 >= end) {
		end = sound_control->music.num_samples;
	}

	sound_control->music.start_loop = start; // * sound_control->music.num_channels;
	sound_control->music.end_loop = end; // * sound_control->music.num_channels;
}

void MusicEnableLoop(sound_control_t* sound_control, int state) {
	if(state != 0) state = 1;
	sound_control->music.looped = state; // 0 is disabled, any other value is true
}

void MusicPlay(sound_control_t* sound_control) {
	switch(sound_control->music.status) {
	case MSS_STOP:
	case MSS_PAUSE:
		sound_control->music.status = MSS_PLAY;
		break;
	case MSS_PLAY:
		break;
	}
}

void MusicPause(sound_control_t* sound_control) {
	switch(sound_control->music.status) {
	case MSS_STOP:
	case MSS_PAUSE:
		break;
	case MSS_PLAY:
		sound_control->music.status = MSS_PAUSE;
		break;
	}
}

void MusicStop(sound_control_t* sound_control) {
	switch(sound_control->music.status) {
	case MSS_STOP:
		break;
	case MSS_PAUSE:
	case MSS_PLAY:
		sound_control->music.status = MSS_STOP;
		sound_control->music.cursor = 0;
		break;
	}
}

void ReadMusicDataToOutput(int16_t* samples, int num_samples, int16_t* output, int channels) {
	for(int i = 0; i < num_samples; i++) {
		switch(channels) {
		case 1:
			output[i * 2] = samples[i] >> 1;
			output[i * 2 + 1] = samples[i] >> 1;
			break;
		case 2:
			output[i * 2] = samples[i * 2] >> 1;
			output[i * 2 + 1] = samples[i * 2 + 1] >> 1;
			break;
		}
	}
}

ma_int16 buff[4096] = { 0 };

void SoundBufferPlayback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) {
	sound_control_t* snd_control = (sound_control_t*)pDevice->pUserData;
	sound_buffer_container_t* all_buffer = &snd_control->sounds;
	ma_int16* buf = (ma_int16*)pOutput;

	assert(0 != all_buffer);
	assert(0 != all_buffer->sound_buffers);

	int num_sbuffers = all_buffer->cnt;
	
	// First test music
	if(snd_control->music.status == MSS_PLAY) {
		// Take loop
		uint64_t start = snd_control->music.start_loop;
		uint64_t end = (1 == snd_control->music.looped) ? snd_control->music.end_loop : snd_control->music.num_samples;
		uint64_t cursor = snd_control->music.cursor;
		int16_t* samples = snd_control->music.samples;
		int channels = snd_control->music.num_channels;

		int64_t available = (end - snd_control->music.cursor); // End - current_pos
		uint64_t to_read = (available > frameCount) ? frameCount : available; // How much to read (1st read)
		//printf("start %d, end %d\n", start, end);
		//printf("cursor: %d, to_read: %d, available: %d\n", cursor, to_read, available);
		int idx = 0;
		if(0 < to_read) {
			ReadMusicDataToOutput(samples + cursor * channels, to_read, buf, channels);
			cursor += to_read;	
		}
		if(1 == snd_control->music.looped) {
			if(cursor >= end) {
				cursor = start;
				ReadMusicDataToOutput(samples + cursor * channels, frameCount - to_read, buf + to_read * channels, channels);
				cursor += frameCount - to_read;
			}
		} else {
			if(frameCount > available) MusicStop(snd_control);
		}
		snd_control->music.cursor = cursor;
	}

	// Go over all sound buffers
	for (int b = 0; b < num_sbuffers; b++) {
		const sound_buffer_t* sbuffer = &all_buffer->sound_buffers[b];
		const int num_buffers = sbuffer->cnt;
		assert(0 != sbuffer);

		// Go over each sound instance
		for (int i = 0; i < num_buffers; i++) {
			audio_buffer_t* abuffer = &sbuffer->buffers[i];
			// Do not play whatever is stoped or paused
			if (abuffer->status == AB_STOP) continue;

			ma_uint64 frms = ma_audio_buffer_read_pcm_frames(&abuffer->buffer_info, buff, frameCount, false);
			if (0 < frms) {
				int maxr = 0, maxl = 0, acum = 0;

				// Keep volume at bay
				switch (sbuffer->channels) {
				case 1:
					for (int j = 0; j < frms; j++) {
						acum = buff[j];
						maxr = (buf[j * 2] + acum * (0.5 - abuffer->place));
						maxl = (buf[j * 2 + 1] + acum * (0.5 + abuffer->place));

						buf[j * 2] = (maxr > INT16_MAX) ? INT16_MAX : (maxr < INT16_MIN) ? INT16_MIN : maxr;
						buf[j * 2 + 1] = (maxl > INT16_MAX) ? INT16_MAX : (maxl < INT16_MIN) ? INT16_MIN : maxl;
					}
					break;
				case 2:
					for (int j = 0; j < frms; j++) {
						maxr = (buf[j * 2] + buff[j * 2] * (0.5 - abuffer->place));
						maxl = (buf[j * 2 + 1] + buff[j * 2 + 1] * (0.5 + abuffer->place));

						buf[j * 2] = (maxr > INT16_MAX) ? INT16_MAX : (maxr < INT16_MIN) ? INT16_MIN : maxr;
						buf[j * 2 + 1] = (maxl > INT16_MAX) ? INT16_MAX : (maxl < INT16_MIN) ? INT16_MIN : maxl;
					}
					break;
				}
			}
			else {
				// It ended? just stop it
				abuffer->status = AB_STOP;
				abuffer->place = 0.0f;
			}
		}
	}
}
