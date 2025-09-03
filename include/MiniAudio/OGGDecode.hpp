#ifndef OGGDECODE_INCLUDED
#define	OGGDECODE_INCLUDED

#include <stdint.h>

struct vorbis_data_t {
	uint64_t sample_count = 0;
	int16_t* sample_data = nullptr;
	int channels = 0;
	int frequency = 0;
};

struct vorbis_stream_t {
	uint64_t sample_count = 0;
	uint64_t pos = 0;
	uint64_t loop_begin = 0;
	uint64_t loop_end = 0;
	bool is_loop = false;
	int channels = 0;
	int frequency = 0;
	int16_t* sample_data = nullptr;
	void* vorbis_file = nullptr;
};

bool LoadVorbisFile(const char* filename, vorbis_data_t* vorbis_data);
void ReleaseVorbisData(vorbis_data_t* vorbis_data);

bool BeginVorbisStream(const char* filename, vorbis_stream_t* vorbis_stream);
void EndVorbisStream(vorbis_stream_t* vorbis_stream);

void ReadVorbisChunk(vorbis_stream_t* vorbis_stream, int16_t* samples, int count);
void SetLoopPoints(vorbis_stream_t* vorbis_stream, uint64_t begin, uint64_t end);
void SetLoop(vorbis_stream_t* vorbis_stream, bool is_loop);

#endif