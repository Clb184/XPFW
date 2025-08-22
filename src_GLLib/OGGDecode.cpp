#include "MiniAudio/OGGDecode.hpp"
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>
#include "Output.hpp"
#include <assert.h>
#include <string>

namespace Clb184 {

	bool LoadVorbisFile(const char* filename, vorbis_data_t* vorbis_data) {
		char buf[256] = "";
		sprintf(buf, "Loading Vorbis file \"%s\"", filename);
		LOG_INFO(buf);
		assert(nullptr != vorbis_data);

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
        vorbis_data->sample_count = sample_count;
        vorbis_data->sample_data = samples;
        vorbis_data->channels = channels;
        vorbis_data->frequency = vi->rate;
        ov_clear(vf);
        delete vf;
        return true;
	}

	void ReleaseVorbisData(vorbis_data_t* vorbis_data) {
        LOG_INFO("Releasing Vorbis data");
        assert(nullptr != vorbis_data);
        delete[] vorbis_data->sample_data;
        memset(vorbis_data, 0, sizeof(vorbis_data_t));
	}

	bool BeginVorbisStream(const char* filename, vorbis_stream_t* vorbis_stream) {
        char buf[256] = "";
        sprintf(buf, "Starting Vorbis file stream \"%s\"", filename);
        LOG_INFO(buf);
        assert(nullptr != vorbis_stream);

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

        // Save info and release the file
        vorbis_stream->sample_count = ov_pcm_total(vf, -1);
        vorbis_stream->channels = vi->channels;
        vorbis_stream->frequency = vi->rate;
        vorbis_stream->is_loop = false;
        vorbis_stream->vorbis_file = vf;
        return true;
	}

	void EndVorbisStream(vorbis_stream_t* vorbis_stream) {
        LOG_INFO("Ending Vorbis file stream");
        assert(nullptr != vorbis_stream);

        ov_clear((OggVorbis_File*)vorbis_stream->vorbis_file);
        delete (OggVorbis_File*)vorbis_stream->vorbis_file;
        memset(vorbis_stream, 0, sizeof(vorbis_stream_t));
	}

	void ReadVorbisChunk(vorbis_stream_t* vorbis_stream, int16_t* samples, int count, int channels) {

	}
}