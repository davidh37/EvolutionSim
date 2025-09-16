#version 330 core
#extension GL_ARB_explicit_uniform_location : require

in vec2 Texcoord;
in vec3 color;
out vec4 out_Color;
layout(location = 1) uniform sampler2D tex;

void main(){
    //out_Color = vec4(vec3(gl_FragCoord.z), 1.0);
    //out_Color = vec4(1.0f, 0.0f, 0.0f, 1.0f);
    vec4 tex_color = texture(tex, Texcoord);
    out_Color = vec4(tex_color) * vec4(color, 1.0f);
}
