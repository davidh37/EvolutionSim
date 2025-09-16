#pragma once

#include "glad/glad/glad.h"
#include <SDL2/SDL.h>
#include "common.hpp"


struct Texture {
    SDL_Surface *surf;
    GLuint id = 0;

    void create(string const &path, bool flip);
    void load();
    uint8_t* get_pixels(int &pitch);
    void convert_black_to_alpha();
    void upload(bool interpolate);
    void destroy();
};
