#include <MiniAudio/Sound.hpp>
#include <stdint.h>
#include <assert.h>
#include "Output.hpp"

namespace Clb184 {
	bool InitializeSoundControl(sound_control_t* sound_control, int num_sound_buffers) {
		LOG_INFO("Initializing Sound Control");
		assert(nullptr != sound_control);

		ma_device_config cfg;
		cfg = ma_device_config_init(ma_device_type_playback);
		cfg.playback.format = ma_format_s16;
		cfg.playback.channels = 2;
		cfg.sampleRate = 44100;
		cfg.dataCallback = SoundBufferPlayback;
		cfg.pUserData = &sound_control->sounds;
		sound_control->device;

		// Initialize device
		if (MA_SUCCESS != ma_device_init(nullptr, &cfg, &sound_control->device)) {
			ma_decoder_uninit(&sound_control->decoder);
			return false;
		}

		// Allocate all sound buffers needed
		sound_control->sounds.cnt = num_sound_buffers;
		sound_control->sounds.sound_buffers = new sound_buffer_t[num_sound_buffers];
		if (nullptr == sound_control->sounds.sound_buffers) return false;

		// Now start sound
		if (MA_SUCCESS != ma_device_start(&sound_control->device)) {
			ma_device_uninit(&sound_control->device);
			ma_decoder_uninit(&sound_control->decoder);
			return false;
		}

		return true;
	}

	void SetSoundMasterVolume(sound_control_t* sound_control, float level) {
		assert(nullptr != sound_control);
		ma_device_set_master_volume(&sound_control->device, level);
	}

	bool CreateSoundBuffer(sound_control_t* sound_control, int index, int cnt, const char* filename) {
		char buf[256] = "";
		sprintf(buf, "Creating Sound Buffer (%d) for \"%s\" x%d", index, filename, cnt);
		LOG_INFO(buf);
		assert(nullptr != sound_control);
		assert(sound_control->sounds.cnt > index);

		ma_result res;
		ma_audio_buffer_config cfg;
		ma_uint64 frames;
		ma_decoder_config dec_cfg;

		// Init data
		dec_cfg = ma_decoder_config_init(ma_format_s16, 2, 44100);
		res = ma_decoder_init_file(filename, &dec_cfg, &sound_control->decoder);
		if (MA_SUCCESS != res) return false;
		res = ma_decoder_get_available_frames(&sound_control->decoder, &frames);
		if (MA_SUCCESS != res) return false;

		ma_int16* samples = new ma_int16[frames * 2 + 2];
		ma_int16* offset = samples;
		ma_uint64 frm = 0;
		ma_uint64 frm_total = 0;

		// Completely read all data
		do {
			ma_decoder_read_pcm_frames(&sound_control->decoder, offset, 4096, &frm);
			offset += frm * 2;
			frm_total += frm;
		} while (frm);

		cfg = ma_audio_buffer_config_init(ma_format_s16, 2, frames, samples, nullptr);

		sound_buffer_t* sound_buffer = &sound_control->sounds.sound_buffers[index];

		sound_buffer->cnt = cnt;
		sound_buffer->data = samples;
		sound_buffer->buffers = new audio_buffer_t[cnt];
		if (nullptr == sound_buffer->buffers) return false;
		for (int i = 0; i < cnt; i++) {
			res = ma_audio_buffer_init(&cfg, &sound_buffer->buffers[i].buffer_info);
		}
	}

	void DestroySoundBuffer(sound_control_t* sound_control, int index) {
		LOG_INFO("Destroying Sound Buffer");
		assert(nullptr != sound_control);
		assert(nullptr != sound_control->sounds.sound_buffers);
		assert(sound_control->sounds.cnt > index);
		assert(nullptr != sound_control->sounds.sound_buffers[index].data);
		
		// Proceeed with the sound buffer
		sound_buffer_t* sound_buffer = &sound_control->sounds.sound_buffers[index];

		sound_buffer->cnt = 0; // Zero available buffers
		delete sound_buffer->buffers; // Delete all buffers
		sound_buffer->buffers = nullptr;
		delete sound_buffer->data; // Delete the PCM data as is not needed anymore
		sound_buffer->data = nullptr;
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

	void PlaySndX(sound_control_t* sound_control, int index, float x) {
		const int cnt = sound_control->sounds.sound_buffers[index].cnt;
		constexpr float div_place = 1.0f / 300.0f;
		audio_buffer_t* buffer = sound_control->sounds.sound_buffers[index].buffers;
		for (int i = 0; i < cnt; i++) {
			if (buffer[i].status == AB_PLAY) continue;
			buffer[i].status = AB_PLAY;
			const float place = x * div_place;
			buffer[i].place = (1.0f > place) ? 1.0f : (place < 0.0f) ? 0.0f : place;
			ma_audio_buffer_seek_to_pcm_frame(&buffer[i].buffer_info, 0);
			break;
		}
	}

	void SoundBufferPlayback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) {
		sound_buffer_container_t* all_buffer = (sound_buffer_container_t*)pDevice->pUserData;
		ma_int16* buf = (ma_int16*)pOutput;
		ma_int16 buff[4096] = { 0 };

		assert(nullptr != all_buffer);
		assert(nullptr != all_buffer->sound_buffers);
		
		int num_sbuffers = all_buffer->cnt;

		// Go over all sound buffers
		for (int b = 0; b < num_sbuffers; b++) {
			const sound_buffer_t* sbuffer = &all_buffer->sound_buffers[b];
			const int num_buffers = sbuffer->cnt;
			assert(nullptr != sbuffer);

			// Go over each sound instance
			for (int i = 0; i < num_buffers; i++) {
				audio_buffer_t* abuffer = &sbuffer->buffers[i];
				// Do not play whatever is stoped or paused
				if (abuffer->status == AB_STOP) continue;
				
				ma_uint64 frms = ma_audio_buffer_read_pcm_frames(&abuffer->buffer_info, buff, frameCount, false);
				if (frms) {
					// Keep volume at bay
					for (ma_uint64 j = 0; j < frms; j++) {
						const int maxr = (buf[j * 2] + ((buff[j * 2]) >> 1) * (0.5 - abuffer->place));
						const int maxl = (buf[j * 2 + 1] + ((buff[j * 2 + 1]) >> 1) * (0.5 + abuffer->place));

						buf[j * 2] = (maxr > INT16_MAX) ? INT16_MAX : (maxr < INT16_MIN) ? INT16_MIN : maxr;
						buf[j * 2 + 1] = (maxl > INT16_MAX) ? INT16_MAX : (maxl < INT16_MIN) ? INT16_MIN : maxl;
					}
				}
				else {
					// It ended? just stop it
					abuffer->status = AB_STOP;
				}
			}
		}
	}
}