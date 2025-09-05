#ifndef FONT_INCLUDED
#define FONT_INCLUDED

#include <GL/glew.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	float x1, x2, y1, y2; // Pos in UV
	float t, b, l, r;
	float adv;
} glyph_info_t;
	
typedef FT_Face font_descriptor_t;

typedef struct {
	GLuint font_atlas;
	float size; // Size for the font
	float w, h; // Width and height of the texture
	glyph_info_t glyphs[128 - 32]; // I just use the very basics (English)... For now
	GLuint vbuffer, varray; // Vertex Buffer and Vertex Array to bind for drawing
	GLuint render_surface, render_target;
} font_t;


bool InitializeFreeType(FT_Library* library); // Just a way to say "FT_Init_Library == FT_Err_Ok"
bool UninitializeFreeType(FT_Library library);

bool LoadFontFromFile(FT_Library library, font_descriptor_t* font, const char* file);
bool CreateFontWithAtlas(font_descriptor_t font_desc, font_t* font, float size); // Size is in pixels for each Glyph (limit), but needs some adjustments...

void DrawString(font_t* font, float x, float y, const char* string);

#ifdef __cplusplus
}
#endif

#endif