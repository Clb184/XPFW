#version 450 core

out vec4 out_color;

//Texture2D g_Texture : register(t0); 
uniform sampler2D g_Texture;

in struct vsout {
    vec4 position;
    vec4 color;
    vec2 texcoord;
} VS_OUT;

layout(std430, binding = 2) readonly buffer WorldLight {
    vec4 Ambient;
};

void main() {
    vec4 finalcolor = VS_OUT.color; //texture2D(g_Texture, VS_OUT.texcoord);
    finalcolor += Ambient;
    out_color = clamp(finalcolor, 0.0f, 1.0f);
}