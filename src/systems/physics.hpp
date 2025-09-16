#pragma once
#include "engine/common.hpp"
#include "ecs.hpp"

namespace physics {

    void initialize();

    void cleanup();

    void update(uint64 tick);

    // ================== read only =============

    ecs::CID findBody(vec2 position);

    struct RaycastInfo {
        ecs::CID hit_id = ecs::INVALID_CID;
        float distanceSq = 0.0f;
    };

    RaycastInfo raycast(vec2 position, vec2 normal, float range);
}
 
