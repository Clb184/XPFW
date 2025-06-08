#version 450 core

vec4 out_color;

//Texture2D g_Texture : register(t0);                                                  
uniform sampler2D g_Texture;                                  
                                                                                            
in struct vsout {                                                             
    vec4 position;                                          
    vec4 color;                                                   
    vec2 texcoord;                                             
    vec3 normals;                                                
    float fograte;                                                    
    vec4 pos2;                                                   
} VS_OUT;                                                                                 
                                                                                             
layout(std140, binding = 2) uniform WorldLight {                                                  
    vec4 GlobalLightRot;                                                           
    vec4 Ambient;                                                                  
    vec4 FogColor;                                                                 
    vec3 GlobalLightColor;                                                         
    float SpecularPower;
    vec4 CameraPos;
};                                                                                
                                                                                             
void main() {                                        
    vec4 finalcolor = vec4(1.0); //texture2D(g_Texture, VS_OUT.texcoord);   
    vec3 norm = normalize(VS_OUT.normals);                   
    // max(dot(norm, -normalize(GlobalLightRot.xyz)), 0.0f)
    vec4 factor = vec4(vec3(max(dot(norm, -normalize(GlobalLightRot.xyz)), 0.0f)),1.0f);  
    float specval = pow(max(0.0, dot(normalize(CameraPos.xyz - VS_OUT.pos2.xyz), normalize(reflect(-GlobalLightRot.xyz, norm)))), SpecularPower);
    finalcolor.xyz = finalcolor.xyz + clamp(specval, 0.0, 1.0);
    finalcolor.xyz *= GlobalLightColor;
    finalcolor *= factor * VS_OUT.color ;                                              
    finalcolor += Ambient;                                                           
    finalcolor.rgb = mix(finalcolor.rgb, FogColor.rgb, VS_OUT.fograte);              
    out_color = clamp(finalcolor, 0.0f, 1.0f);                                                     
}                                                                                    ;