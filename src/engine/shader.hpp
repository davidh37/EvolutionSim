#pragma once

#include "glad/glad/glad.h"
#include "common.hpp"

struct Shader {
    GLuint id;
    GLuint vert_id, frag_id;

    void compile(string const& vert_location, string const& frag_location);
    void load();
    void setUniformInteger(int location, int val);
    void setUniformMat4(int location, mat4 val);
    void destroy();
};


