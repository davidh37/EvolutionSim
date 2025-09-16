#version 330 core
#extension GL_ARB_explicit_uniform_location : require

layout(location = 0) in vec3 in_Position;
layout(location = 1) in vec3 in_Color;
layout(location = 2) in vec2 in_Texcoord;
layout(location = 3) in vec2 instance_offset;
layout(location = 4) in float instance_scale;
layout(location = 5) in vec3 instance_color;

layout(location = 0) uniform mat4 mvp;

out vec3 color;

void main(){
    color = instance_color;
    vec3 pos = (in_Position * instance_scale) + vec3(instance_offset[0], instance_offset[1], 0.0);
    gl_Position = mvp * vec4(pos, 1.0);
}


