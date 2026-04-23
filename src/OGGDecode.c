#include "MiniAudio/OGGDecode.h"
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>
#include "Output.h"
#include <assert.h>
#include <string.h>

typedef struct {
	char* source_data;
	char* move_data;
	size_t offset;	
	size_t size;
} vorbis_mem_t;

size_t ReadVorbisData(void* data, size_t size, size_t nmemb, void* source) {
	//printf("ReadVorbisData()\n");
	//printf("data: %p size: %lld nmemb: %lld source: %p\n", data, size, nmemb, source);
#ifdef _MSC_VER
#pragma message("Adding debug break (MSVC)")
	//__debugbreak();
#endif
	if(0 == source) return -1;
	vorbis_mem_t* vm = (vorbis_mem_t*)source;
	//printf("offset: %lld size: %lld\n", vm->offset, vm->size);
	size_t to_read = (nmemb * size) > (vm->size - vm->offset) ? vm->size - vm->offset : nmemb * size;
	//printf("to_read returned: %lld\n", to_read);
	memcpy(data, vm->move_data, to_read);
	vm->move_data += to_read;
	vm->offset += to_read;
	return to_read;
}

int SeekVorbisData(void* source, ogg_int64_t offset, int whence) {
	//printf("SeekVorbisData()\n");
	//printf("source: %p offset: %I64d whence: %d\n", source, offset, whence);
#ifdef _MSC_VER
#pragma message("Adding debug break (MSVC)")
	//__debugbreak();
#endif
	if(0 == source) return -1;
	vorbis_mem_t* vm = (vorbis_mem_t*)source;
	switch(whence) {
		case 0: // SET
			vm->move_data = vm->source_data + offset;
			vm->offset = offset;
			break;
		case 1: // CUR
			vm->move_data += offset;
			vm->offset += offset;
			break;
		case 2: // END
			vm->move_data = vm->source_data + vm->size + offset;
			vm->offset = vm->size + offset;
			break;
		default:
		       printf("Whence was: %d\n", whence);	
			return -1;
	}
	return 0;
}

long TellVorbisPos(void* source) {
	//printf("TellVorbisPos()\n");
	//printf("source: %p\n", source);
#ifdef _MSC_VER
#pragma message("Adding debug break (MSVC)")
	//__debugbreak();
#endif
	if(0 == source) return -1;
	vorbis_mem_t* vm = (vorbis_mem_t*)source;
	//printf("Offset: %lld\n", vm->offset);
	return vm->offset;
}

bool LoadVorbisFile(const char* filename, vorbis_data_t* vorbis_data) {
	char buf[256] = "";
	sprintf(buf, "Loading Vorbis file \"%s\"", filename);
	LOG_INFO(buf);
	assert(0 != vorbis_data);

       // Decode all the data with this simple function
       FILE* fp = 0;
       OggVorbis_File* vf = 0;
       vorbis_info* vi = 0;
	
       if (0 == (vf = calloc(1, sizeof(OggVorbis_File)))) return false;

       if(0 != ov_fopen(filename, vf)) {
		sprintf(buf, "Vorbis file \"%s\" failed to initialize", filename);
		LOG_ERROR(buf);
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
bool LoadVorbisFromMemory(char* data, size_t size, vorbis_data_t* vorbis_data) {
	char buf[256] = "";
	int result = 0;
	sprintf(buf, "Loading Vorbis data from memory (%lld bytes)", size);
	LOG_INFO(buf);
	assert(0 != vorbis_data);
	ov_callbacks data_read = { 0 };
	vorbis_mem_t mem = { 0 };
	data_read.read_func = ReadVorbisData;
	data_read.seek_func = SeekVorbisData;
	data_read.tell_func = TellVorbisPos;
	
	mem.source_data = data;
	mem.move_data = data;
	mem.offset = 0;
	mem.size = size;

       // Decode all the data with this simple function
       OggVorbis_File* vf = 0;
       vorbis_info* vi = 0;
	
       if (0 == (vf = calloc(1, sizeof(OggVorbis_File)))) return false;

       LOG_INFO("Testing file");
       //printf("data: %p vf: %p mem: %p\n", data, vf, &mem);
       result = ov_open_callbacks(&mem, vf, data, size, data_read);
       if (result < 0) {
           sprintf(buf, "Invalid OGG data ID: %d", result);
           free(vf);
           return false;
       }

       LOG_INFO("Getting vorbis file info");
       vi = ov_info(vf, -1);
       int64_t sample_count = ov_pcm_total(vf, -1);
       int channels = vi->channels;
       sprintf(buf, "Samples: %I64d Channels: %d", sample_count, channels);
       LOG_INFO(buf);

	LOG_INFO("Allocating sample data");       
        int16_t* samples = calloc(sizeof(int16_t), sample_count * channels);
	//printf("samples: %p\n", samples);

       if (0 == samples) {
           ov_clear(vf);
           free(vf);
           return false;
       }

       // Read until filling all the buffer
       int bitstr = 0;
       long ret = 0;
       char* seek = (char*)samples;
	//printf("seek: %p\n", seek);
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
