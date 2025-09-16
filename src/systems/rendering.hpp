#pragma once
#include "engine/common.hpp"
#include "ecs.hpp"

/* Read only system */
namespace rendering {

    void initialize();

    void cleanup();

    void update(float real_delta);

    void moveCamera(vec3 direction);

    void setRenderMode(int mode);

    void setStateInfo(string info);

    vec2 cursorToWorld(int mx, int my);

    void resize(int x, int y);
}


