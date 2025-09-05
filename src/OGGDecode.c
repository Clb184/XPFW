#include "MiniAudio/OGGDecode.h"
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>
#include "Output.h"
#include <assert.h>
#include <string.h>

bool LoadVorbisFile(const char* filename, vorbis_data_t* vorbis_data) {
	char buf[256] = "";
	sprintf(buf, "Loading Vorbis file \"%s\"", filename);
	LOG_INFO(buf);
	assert(0 != vorbis_data);

       // Decode all the data with this simple function
       FILE* fp = 0;
       OggVorbis_File* vf = 0;
       vorbis_info* vi = 0;

       if (!(fp = fopen(filename, "rb"))) return false;

       if (0 == (vf = calloc(1, sizeof(OggVorbis_File)))) return false;

       if ((ov_open_callbacks(fp, vf, NULL, 0, OV_CALLBACKS_NOCLOSE) < 0)) {
           printf("Invalid OGG file.\n");
           free(vf);
           return false;
       }

       vi = ov_info(vf, -1);
       int64_t sample_count = ov_pcm_total(vf, -1);
       int channels = vi->channels;
	
        int16_t* samples = calloc(sizeof(int16_t), sample_count * channels);
       //memset(samples, 0, sizeof(int16_t) * sample_count * channels);

       if (0 == samples) {
           ov_clear(vf);
           free(vf);
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
       free(vf);
       return true;
}

void ReleaseVorbisData(vorbis_data_t* vorbis_data) {
       LOG_INFO("Releasing Vorbis data");
       assert(0 != vorbis_data);
       free(vorbis_data->sample_data);
       memset(vorbis_data, 0, sizeof(vorbis_data_t));
}

bool BeginVorbisStream(const char* filename, vorbis_stream_t* vorbis_stream) {
       char buf[256] = "";
       sprintf(buf, "Starting Vorbis file stream \"%s\"", filename);
       LOG_INFO(buf);
       assert(0 != vorbis_stream);

       int16_t* samples = calloc(sizeof(int16_t), 441 * 1000);

       // Decode all the data with this simple function
       FILE* fp = 0;
       OggVorbis_File* vf = 0;
       vorbis_info* vi = 0;
       if (!(fp = fopen(filename, "rb"))) return false;

       if (0 == (vf = calloc(sizeof(OggVorbis_File), 1))) return false;

       if ((ov_open_callbacks(fp, vf, NULL, 0, OV_CALLBACKS_NOCLOSE) < 0)) {
           printf("Invalid OGG file.\n");
           free(vf);
           return false;
       }

       vi = ov_info(vf, -1);

       // Save info and release the file
       vorbis_stream->sample_count = ov_pcm_total(vf, -1);
       vorbis_stream->pos = 0;
       vorbis_stream->channels = vi->channels;
       vorbis_stream->frequency = vi->rate;
       vorbis_stream->is_loop = false;
       vorbis_stream->vorbis_file = vf;
       return true;
}

void EndVorbisStream(vorbis_stream_t* vorbis_stream) {
       LOG_INFO("Ending Vorbis file stream");
       assert(0 != vorbis_stream);

       ov_clear((OggVorbis_File*)vorbis_stream->vorbis_file);
       free(vorbis_stream->vorbis_file);
       memset(vorbis_stream, 0, sizeof(vorbis_stream_t));
}

void ReadVorbisChunk(vorbis_stream_t* vorbis_stream, int16_t* samples, int count) {
       assert(0 != vorbis_stream);
       int bitstream = 0;
       const int total_size = count * 2 * sizeof(int16_t);
       int readen = 0;
       int total_readen = 0;
       int channels = vorbis_stream->channels;
       if (vorbis_stream->is_loop) {
           uint64_t pos = vorbis_stream->pos;
           uint64_t begin = vorbis_stream->loop_begin;
           uint64_t end = vorbis_stream->loop_end;
           const uint64_t remainder_size = (end - pos) * sizeof(int16_t) * channels;
           if ((end - pos) < count) {
               do {
                   readen = ov_read((OggVorbis_File*)vorbis_stream->vorbis_file, (char*)(samples + total_readen), remainder_size, 0, sizeof(int16_t), 1, &bitstream);
                   total_readen += readen;

                   if ((total_readen < total_size) && (readen == 0)) {
                       ov_pcm_seek((OggVorbis_File*)vorbis_stream->vorbis_file, vorbis_stream->loop_begin);
                   }
               } while (total_readen < total_size);
               pos += total_readen;
           }
           else {
               do {
                   readen = ov_read((OggVorbis_File*)vorbis_stream->vorbis_file, (char*)(samples + total_readen), remainder_size, 0, sizeof(int16_t), 1, &bitstream);
                   total_readen += readen;

                   if ((total_readen < total_size) && (readen == 0)) {
                       ov_pcm_seek((OggVorbis_File*)vorbis_stream->vorbis_file, vorbis_stream->loop_begin);
                   }
               } while (total_readen < total_size);
               pos += total_readen;
           }

       }
       else {
           do {
               readen = ov_read((OggVorbis_File*)vorbis_stream->vorbis_file, (char*)(samples + total_readen), total_size, 0, sizeof(int16_t), 1, &bitstream);
               total_readen += readen;
           } while (total_readen < total_size && (readen > 0));
           vorbis_stream->pos += total_readen;
       }
}

   void SetLoopPoints(vorbis_stream_t* vorbis_stream, uint64_t begin, uint64_t end) {
       assert(0 != vorbis_stream);
       uint64_t rbegin = 0, rend = 0;
       if (begin < end) {
           rbegin = begin;
           rend = end;
       }
       else {
           rbegin = end;
           rend = begin;
       }
       vorbis_stream->loop_begin = (rbegin < 0) ? 0 : (rbegin > vorbis_stream->sample_count) ? vorbis_stream->sample_count : rbegin;
       vorbis_stream->loop_end = (rend < 0) ? 0 : (rend > vorbis_stream->sample_count) ? vorbis_stream->sample_count : rend;
   }

   void SetLoop(vorbis_stream_t* vorbis_stream, bool is_loop) {
       assert(0 != vorbis_stream);
       vorbis_stream->is_loop = is_loop;
   }
