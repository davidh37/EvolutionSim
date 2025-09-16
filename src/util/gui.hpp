#pragma once
#include "engine/common.hpp"

namespace gui{
    static string TERMINAL_COLOR = "\033[1;30m";

    void initialize();

    void cleanup();

    void drawBox(float x, float y, float w, float h, vec3 color);

    void drawChar(int character, float x, float y, float w, float h, vec3 color);

    void drawString(string text, float x, float y, float w, float h, vec3 color);

    void drawStringUnscaled(string text, float x, float y, float scale, vec3 color);

    void render();
}



