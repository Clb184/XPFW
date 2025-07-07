#include "OpenGL/Texture.hpp"
#include "IO.hpp"
#include "Output.hpp"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <png.h>

namespace Clb184 {
	void CreateTexture(GLuint* tex_unit, GLsizei w, GLsizei h, char* pixel_data) { 
		assert(nullptr != tex_unit);
		// Create test texture for png
		GLuint tex_png;
		glCreateTextures(GL_TEXTURE_2D, 1, &tex_png);
		glBindTexture(GL_TEXTURE_2D, tex_png);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixel_data); // Always using RGBA 8 bit per channel
		GL_ERROR();
		*tex_unit = tex_png;
	}

	bool LoadTextureFromFile(const char* name, GLuint* tex_unit) {
		LOG_INFO("Creating texture from file");
		assert(nullptr != tex_unit);
		const char* png_title = name;
		png_bytep chardata = nullptr;
		if (false == LoadDataFromFile(png_title, (void**)&chardata, nullptr)) return 0;
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
			return 0;
		}
		free(chardata);
		png_structp png_reader = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
		if (nullptr == png_reader) return 0;

		png_infop png_info = png_create_info_struct(png_reader);
		if (nullptr == png_info) {
			png_destroy_read_struct(&png_reader, nullptr, nullptr);
			return 0;
		}

		FILE* fp = fopen(png_title, "rb");
		png_set_gamma(png_reader, PNG_DEFAULT_sRGB, PNG_DEFAULT_sRGB);
		png_set_alpha_mode(png_reader, PNG_ALPHA_PNG, PNG_DEFAULT_sRGB);
		png_init_io(png_reader, fp);
		png_read_info(png_reader, png_info);
		unsigned int
			width = png_get_image_width(png_reader, png_info),
			height = png_get_image_height(png_reader, png_info),
			channels = png_get_channels(png_reader, png_info);

		char* pixel_data = (char*)malloc(width * height * channels);
		if (setjmp(png_jmpbuf(png_reader))) {
			png_destroy_read_struct(&png_reader, &png_info, nullptr);
		}
		char** ppRows = (char**)malloc(sizeof(char*) * width);
		for (int i = 0; i < height; i++) {
			ppRows[i] = &pixel_data[channels * width * (height - 1 - i)];
		}
		png_set_rows(png_reader, png_info, (png_bytepp)ppRows);
		png_read_image(png_reader, (png_bytepp)ppRows);
		png_read_end(png_reader, png_info);
		png_destroy_read_struct(&png_reader, &png_info, nullptr);
		CreateTexture(tex_unit, width, height, pixel_data);
		free(pixel_data);
		free(ppRows);
	}
}