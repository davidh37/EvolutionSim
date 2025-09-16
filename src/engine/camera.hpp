#pragma once

#include "common.hpp"

namespace camera {
    void setProjection(float fov, float near, float far);

    void setOrthographic(float left, float right, float bottom, float top);

    mat4 getProjectionMatrix();

    mat4 getViewMatrix();

    void setScreenSize(int w, int h);

    vec3 screenToWorld(int x, int y);


    void setPosition(vec3 pos);

    void addPosition(vec3 vel);

    void setOrientation(float y, float p);

    void addOrientation(float yaw_offset, float pitch_offset);
}

