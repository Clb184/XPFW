#version 450 core
layout(location = 0) in vec3 POSITION;
layout(location = 1) in vec4 COLOR;                                                   
layout(location = 2) in vec2 TEXCOORD;                                             
layout(location = 3) in vec3 NORMAL;                                                
                                                                                    
out struct vsout {                                                             
    vec4 position;                                          
    vec4 color;                                                   
    vec2 texcoord;                                             
    vec3 normals;                                                
    float fograte;                                                    
    vec4 pos2;                                                   
} VS_OUT;                                                                          
                                                                                    
layout(std140, binding = 0) uniform CameraMatrix {                                       
    mat4 camMat;        // 0, 1, 2 ,3                                                       
    mat4 View;          // 4, 5, 6 ,7                          
    mat4 Projection;    // 8, 9, 10, 11                                             
    vec2 Fog;           // 12                                   
    vec2 ExpFog;        // 13                                         
    float Density;      // 14                                              
};                                                                          
                                                                                    
layout(std140, binding = 1) uniform Normals { 
    mat4 model;
    mat3 normMat;                                                       
};                                                                          
                                                                                    
void main() {
    vec3 curpos = POSITION;                                         
    vec4 CameraPos = vec4(curpos, 1.0f) * model * View;
    VS_OUT.pos2 = model * vec4(curpos, 1.0f);
    VS_OUT.position = camMat * VS_OUT.pos2;           
    VS_OUT.texcoord = TEXCOORD;                                          
    VS_OUT.color = COLOR;                                                
    VS_OUT.normals = normalize(model * vec4(NORMAL, 1.0)).xyz;           
    VS_OUT.texcoord.y = 1.0f - VS_OUT.texcoord.y;                                 
    VS_OUT.fograte = clamp((length(CameraPos) - Fog.x) / (Fog.y - Fog.x), 0.0f, 1.0f);
    gl_Position = VS_OUT.position;
}