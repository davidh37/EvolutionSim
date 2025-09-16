#pragma once

namespace ecs {
    struct ParticleData {
        bool dead = false;
        float energy = 0.01f;
        vec3 color = COLOR_GREEN;
    };
}
