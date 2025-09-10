#include "OpenGL/Texture.h"
#include "IO.h"
#include "Output.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <png.h>

void CreateTexture(GLuint* tex_unit, GLsizei w, GLsizei h, char* pixel_data) {
	LOG_INFO("Creating Texture");
	assert(0 != tex_unit);
	GLERR;

	// Create texture for png
	GLuint tex_png;
	glCreateTextures(GL_TEXTURE_2D, 1, &tex_png);
	glTextureStorage2D(tex_png, 1, GL_RGBA32F, w, h);
	GL_ERROR();
	glTextureSubImage2D(tex_png, 0, 0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, pixel_data); // Always using RGBA 8 bit per channel
	GL_ERROR();
	*tex_unit = tex_png;
}

typedef struct {
	void* data;
	char* offset;
} virtual_file_t;

void LoadPNGFromMemory(png_structp png, png_bytep buf, size_t size) {
	virtual_file_t* data = (virtual_file_t*)png_get_io_ptr(png);
	assert(0 != data);
	memcpy(buf, data->offset, size);
	data->offset += size;
}

bool LoadTextureFromFile(const char* name, GLuint* tex_unit, int* o_width, int* o_height) {
	LOG_INFO("Loading texture from file");
	assert(0 != tex_unit);

	const char* png_title = name;
	png_bytep chardata = 0;

	// Load data for checking signature
	if (false == LoadDataFromFile(png_title, (void**)&chardata, 0)) {
		char buf[1024] = "";
		sprintf(buf, "\"%s\" does not exist", name); 
		LOG_ERROR(buf); 
		return false;
	}

	// Check for PNG signature
	if (0 == png_sig_cmp(chardata, 0, 16)) {
		char buf[1024] = "";
		sprintf(buf, "Opened PNG image \"%s\"", name);
		LOG_INFO(buf);
	}
	else {
		char buf[1024] = "";
		sprintf(buf, "Failed opening PNG image, \"%s\" is not valid", name);
		LOG_ERROR(buf);
		free(chardata);
		return false;
	}

	// Create the corresponding decoder and reader structs
	png_structp png_reader = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
	if (0 == png_reader) return 0;

	png_infop png_info = png_create_info_struct(png_reader);
	if (0 == png_info) {
		png_destroy_read_struct(&png_reader, 0, 0);
		free(chardata);
		return false;
	}

	// Create a fake file to load texture data from
	virtual_file_t png_file = { chardata, (char*)chardata };
	png_set_gamma(png_reader, PNG_DEFAULT_sRGB, PNG_DEFAULT_sRGB);
	png_set_alpha_mode(png_reader, PNG_ALPHA_PNG, PNG_DEFAULT_sRGB);
	png_set_read_fn(png_reader, 0, LoadPNGFromMemory);
	png_init_io(png_reader, (FILE*)&png_file);
	png_read_info(png_reader, png_info);
	unsigned int
		width = png_get_image_width(png_reader, png_info),
		height = png_get_image_height(png_reader, png_info),
		channels = png_get_channels(png_reader, png_info);

	// Allocate enough space for pixel data (RGBA 32 bits)
	char* pixel_data = (char*)malloc(width * height * channels);
	if (setjmp(png_jmpbuf(png_reader))) {
		png_destroy_read_struct(&png_reader, &png_info, 0);
		free(chardata);
		return false;
	}
	// Same for row pointers and set them
	char** ppRows = (char**)malloc(sizeof(char*) * width);
	for (int i = 0; i < height; i++) {
		ppRows[i] = &pixel_data[channels * width * (i)];
	}
	png_set_rows(png_reader, png_info, (png_bytepp)ppRows);

	// After setup, read all image and all data should be in place
	png_read_image(png_reader, (png_bytepp)ppRows);
	png_read_end(png_reader, png_info);

	// Create the texture with all the retrieved pixel data
	CreateTexture(tex_unit, width, height, pixel_data);

	// And finally, release the decoder struct and free memory
	png_destroy_read_struct(&png_reader, &png_info, 0);
	free(chardata);
	free(pixel_data);
	free(ppRows);

	// Get these properties if requested
	if (0 != o_width) *o_width = width;
	if (0 != o_height) *o_height = height;
	return true;
}

bool CreateEmptyTexture(GLuint* tex_unit,int color) {
	assert(0 != tex_unit);
	int* pixels = calloc(256 * 256 * 4, 1); // Color is 32 bits
	memset(pixels, color, 256 * 256 * 4);
	CreateTexture(tex_unit, 256, 256, (char*)pixels);
	free(pixels);
	return true;
}