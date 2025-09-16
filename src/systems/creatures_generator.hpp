#pragma once
#include "engine/common.hpp"
#include "ecs.hpp"

namespace creatures_generator {
    void initialize();

    void cleanup();

    void update();

    void setBrainMeshing(bool continuous);
}