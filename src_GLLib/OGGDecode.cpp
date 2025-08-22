#include "MiniAudio/OGGDecode.hpp"
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>
#include "Output.hpp"
#include <assert.h>
#include <string>

namespace Clb184 {

	bool LoadVorbisFile(const char* filename, vorbis_data_t* ogg_data) {
		char buf[256] = "";
		sprintf(buf, "Loading Vorbis file \"%s\"", filename);
		LOG_INFO(buf);
		assert(nullptr != ogg_data);

        // Decode all the data with this simple function
        FILE* fp;
        OggVorbis_File* vf;
        vorbis_info* vi;
        if (!(fp = fopen(filename, "rb"))) return false;

        if (nullptr == (vf = new OggVorbis_File())) return false;

        if ((ov_open_callbacks(fp, vf, NULL, 0, OV_CALLBACKS_NOCLOSE) < 0)) {
            printf("Invalid OGG file.\n");
            delete vf;
            return false;
        }

        vi = ov_info(vf, -1);
        int64_t sample_count = ov_pcm_total(vf, -1);
        int channels = vi->channels;
		
		int16_t* samples = new int16_t[sample_count * channels];
        memset(samples, 0, sizeof(int16_t) * sample_count * channels);

        if (nullptr == samples) {
            ov_clear(vf);
            delete vf;
            return false;
        }

        // Read until filling all the buffer
        int bitstr = 0;
        long ret = 0;
        uint64_t size = sizeof(int16_t) * sample_count * channels;
        char* seek = (char*)samples;

        do {
            ret = ov_read(vf, seek, 4096, 0, channels, 1, &bitstr);
            seek += ret;
        } while (ret);

        // Save info and release the file
        ogg_data->sample_count = sample_count;
        ogg_data->sample_data = samples;
        ogg_data->channels = channels;
        ogg_data->frequency = vi->rate;
        ov_clear(vf);
        delete vf;
        return true;
	}

	void ReleaseVorbisData(vorbis_data_t* ogg_data) {
        LOG_INFO("Releasing Vorbis data");
        assert(nullptr != ogg_data);
        delete[] ogg_data->sample_data;
	}

	bool BeginVorbisStream(vorbis_stream_t* vorbis_stream) {
        return true;
	}

	void EndVorbisStream(vorbis_stream_t* vorbis_stream) {

	}

	void ReadVorbisChunk(vorbis_stream_t* vorbis_stream, int16_t* samples, int count, int channels) {

	}
}