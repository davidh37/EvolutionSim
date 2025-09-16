#version 330 core
#extension GL_ARB_explicit_uniform_location : require

layout(location = 0) in vec3 in_Position;
layout(location = 1) in vec3 in_Color;
layout(location = 2) in vec2 in_Texcoord;

layout(location = 0) uniform mat4 mvp;

out vec3 color;

void main(){
    color = in_Color;
    gl_Position = mvp * vec4(in_Position, 1.0);
}


