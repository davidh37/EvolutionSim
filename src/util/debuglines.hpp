#pragma once
#include "engine/common.hpp"

namespace debuglines {
    static string TERMINAL_COLOR = "\033[1;30m";

    void initialize();

    void cleanup();

    void addPoint(vec2 p, vec3 color);

    void render(mat4 transformation, float line_width);
}