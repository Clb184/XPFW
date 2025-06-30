#version 450 core
layout(location = 0) in vec3 POSITION;
layout(location = 1) in vec4 COLOR;
layout(location = 2) in vec2 TEXCOORD;

out struct vsout {
    vec4 position;
    vec4 color;
    vec2 texcoord;
} VS_OUT;

layout(std430, binding = 0) readonly buffer CameraData {
    mat4 camera; // view * proj
};

layout(std430, binding = 1) readonly buffer ModelData {
    mat4 model;
    vec3 posses[24 * 1024 * 1024];
};
                                                                                    
void main() {
    vec3 curpos = POSITION;
    vec4 pos = model * vec4(curpos, 1.0f);
    VS_OUT.position = camera * pos + vec4(posses[16 * 1024 * 1024 - 1], 0.0);
    VS_OUT.texcoord = TEXCOORD;
    VS_OUT.color = COLOR;
    VS_OUT.texcoord.y = 1.0f - VS_OUT.texcoord.y;
    gl_Position = VS_OUT.position;
}