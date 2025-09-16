#version 330 core
#extension GL_ARB_explicit_uniform_location : require

in vec3 color;
out vec4 out_Color;

void main(){
    out_Color = vec4(color, 1.0f);
}
