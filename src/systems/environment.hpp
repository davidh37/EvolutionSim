#pragma once
#include "engine/common.hpp"
#include "ecs.hpp"

namespace environment {
    // full control over: creatures, particles, bodies

    void initialize();

    void cleanup();

    void update(uint64 tick);

    // EXTRA FUNCTIONS

    void spawnFood(vec2 position);
    void addGrowthRate(float rate_delta);
    float getGrowthRate();
}