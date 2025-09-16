#pragma once
#include "engine/common.hpp"
#include "ecs.hpp"

namespace creatures_physics_IO {

    void initialize();

    void cleanup();

    void update();

    float getFeedingRate(ecs::CreatureData &creature, vec2 normal, bool meat);
    
}