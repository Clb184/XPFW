#version 450 core

out vec4 COLOR;

in vec4 uColor;
in vec2 tex_coords;

uniform sampler2D g_Sampler;

void main() {
	COLOR = texture(g_Sampler, tex_coords);
}