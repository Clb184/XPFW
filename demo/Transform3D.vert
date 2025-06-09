#version 450 core
layout(location = 0) in vec3 POSITION;
layout(location = 1) in vec4 COLOR;
layout(location = 2) in vec2 TEXCOORD;

out struct vsout {
    vec4 position;
    vec4 color;
    vec2 texcoord;
} VS_OUT;

layout(std140, binding = 0) uniform CameraMatrix {
    mat4 camMat;        // view * proj
};

uniform mat4 g_Model;
                                                                                    
void main() {
    vec3 curpos = POSITION;
    vec4 pos = g_Model * vec4(curpos, 1.0f);
    VS_OUT.position = camMat * pos;
    VS_OUT.texcoord = TEXCOORD;
    VS_OUT.color = COLOR;
    VS_OUT.texcoord.y = 1.0f - VS_OUT.texcoord.y;
    gl_Position = VS_OUT.position;
}