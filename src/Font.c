#include "OpenGL/Font.h"
#include <assert.h>
#include "Output.h"
#include <Misc/Primitives.h>


bool InitializeFreeType(FT_Library* library) { // Just a way to say "FT_Init_Library == FT_Err_Ok"
	LOG_INFO("Initializing FreeType");
	return FT_Err_Ok == FT_Init_FreeType(library);
}

bool UninitializeFreeType(FT_Library library) {
	return FT_Err_Ok == FT_Done_FreeType(library);
}

bool LoadFontFromFile(FT_Library library, font_descriptor_t* font, const char* font_name) {
	LOG_INFO("Loading Font from file");
	assert(0 != font);
	return FT_Err_Ok == FT_New_Face(library, font_name, 0, font);
}

// Size is in pixels... But needs some adjustments...
bool CreateFontWithAtlas(font_descriptor_t font_desc, font_t* font, float size) {
	LOG_INFO("Creating Font Atlas with size");
	assert(0 != font);
	assert(0 != font_desc);

	FT_Error e;
	FT_Face face = font_desc;

	// Get face data
	e = FT_Set_Char_Size(face, (int)size << 6, (int)size << 6, 64, 64);
	if (FT_Err_Ok != e) { FT_Done_Face(face); return false; }

	int sz = size, glyph_quad = 1;
	while (sz > glyph_quad) { 
		glyph_quad <<= 1; 
	}
	int atlas_size = glyph_quad * 16;

	int x = 0, y = 0;
	float div_width = 1.0f / (float)atlas_size; // 
	const float div_64 = 1.0f / 64.0f;

	int* expanded_pixel_data = calloc(sizeof(int), atlas_size * atlas_size);


	font->size = size;
	font->w = font->h = atlas_size;
	// Get glyph data and pre-calculate some stuff
	for (char c = ' '; c < 127; c++) {
		char ebuf[256] = "";
		int idx = FT_Get_Char_Index(face, c); // Get char index, like name says

		// Load a glyph
		e = FT_Load_Glyph(face, idx, FT_LOAD_DEFAULT);
		if (FT_Err_Ok != e) { FT_Done_Face(face); sprintf(ebuf, "Error loading %c glyph\n", c); LOG_ERROR(ebuf); continue; }

		// Render a glyph
		e = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
		if (FT_Err_Ok != e) { FT_Done_Face(face); sprintf(ebuf, "Error rendering %c glyph\n", c); LOG_ERROR(ebuf); continue; }
		int w = face->glyph->bitmap.width, h = face->glyph->bitmap.rows;

		font->glyphs[c - 32].x1 = x * div_width;
		font->glyphs[c - 32].x2 = (x + w) * div_width;
		font->glyphs[c - 32].y1 = y * div_width;
		font->glyphs[c - 32].y2 = (y + h) * div_width;
		FT_Glyph_Metrics metric = face->glyph->metrics;

		const float y_adjust_top = (metric.horiBearingY) * div_64;
		const float bearing_width = (metric.width + metric.horiBearingX) * div_64;
		const float bearing_x = (metric.horiBearingX) * div_64;
		const float height = (metric.height) * div_64;

		font->glyphs[c - 32].t = -y_adjust_top;
		font->glyphs[c - 32].b = -y_adjust_top + height;
		font->glyphs[c - 32].l = bearing_x;
		font->glyphs[c - 32].r = bearing_width;
		font->glyphs[c - 32].adv = metric.horiAdvance * div_64;

		char* pGlyphPixels = (char*)face->glyph->bitmap.buffer;
		h = (h > glyph_quad) ? glyph_quad : h;
		for (int j = h - 1; j >= 0; j--) {
			for (int i = 0; i < w; i++) {
				int nm = pGlyphPixels[j * w + i];
				expanded_pixel_data[atlas_size * (j + y) + x + i] = (nm << 24) | (nm << 16) | (nm << 8) | nm;
			}
		}
		x += glyph_quad;
		if (x >= atlas_size) { x = 0;	y += glyph_quad; }
		if (y >= atlas_size) { break; }
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	GLuint font_atlas;
	glCreateTextures(GL_TEXTURE_2D, 1, &font_atlas);
	glTextureStorage2D(font_atlas, 1, GL_RGBA32F, atlas_size, atlas_size);
	glTextureSubImage2D(font_atlas, 0, 0, 0, atlas_size, atlas_size, GL_RGBA, GL_UNSIGNED_BYTE, expanded_pixel_data);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 8);

	font->font_atlas = font_atlas;
	
	// Create the vertex buffer and vertex array
	CreateTL2DVertexBuffer(1024L * 6L, 0, GL_DYNAMIC_DRAW, &font->vbuffer, &font->varray); // At least 1024 characters per font (And size)
	return true;
}

void DrawString(font_t* font, float ox, float oy, const char* string) {
	assert(0 != font);

	int len = strlen(string);
	const float block_advance = font->size;
	float x = ox, y = oy + block_advance;

	TLVertex2D* vert = (TLVertex2D*)glMapNamedBuffer(font->vbuffer, GL_WRITE_ONLY);
	const float adv_space = font->glyphs[' ' - 32].adv;
	int ic = 0;
	for (int i = 0; i < len; i++) {
		char c = string[i];
		if (c == '\n') {
			x = 0.0f;
			y += block_advance;
			continue;
		}
		else if (c == ' ') {
			x += adv_space;
			continue;
		}

		glyph_info_t glyph = font->glyphs[c - 32];

		vert[ic * 6].x = x + glyph.l;
		vert[ic * 6].y = y + glyph.t;
		vert[ic * 6].u = glyph.x1;
		vert[ic * 6].v = glyph.y1;
		vert[ic * 6 + 3] = vert[ic * 6];

		vert[ic * 6 + 1].x = x + glyph.r;
		vert[ic * 6 + 1].y = y + glyph.t;
		vert[ic * 6 + 1].u = glyph.x2;
		vert[ic * 6 + 1].v = glyph.y1;

		vert[ic * 6 + 2].x = x + glyph.r;
		vert[ic * 6 + 2].y = y + glyph.b;
		vert[ic * 6 + 2].u = glyph.x2;
		vert[ic * 6 + 2].v = glyph.y2;
		vert[ic * 6 + 4] = vert[ic * 6 + 2];

		vert[ic * 6 + 5].x = x + glyph.l;
		vert[ic * 6 + 5].y = y + glyph.b;
		vert[ic * 6 + 5].u = glyph.x1;
		vert[ic * 6 + 5].v = glyph.y2;

		x += glyph.adv;
		ic++;
	}

	glUnmapNamedBuffer(font->vbuffer);
	glBindTexture(GL_TEXTURE_2D, font->font_atlas);
	glBindVertexArray(font->varray);
	glDrawArrays(GL_TRIANGLES, 0, ic * 6);
}
