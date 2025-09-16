#pragma once
#include "ecs.hpp"
#include "config.hpp"

namespace ecs {

    struct CollisionInfo {
        CID other_id = INVALID_CID;              // physics body
        vec2 other_vector = vec2();
    };

    struct PhysicsBody {
            // physical "constants"
            float friction = config::PHYSICS_FRICTION;
            float angular_friction = config::PHYSICS_ANGULAR_FRICTION;
            float mass = 1.0f;
            float radius = 0.2f;

            // physical variables
            vec2 position = vec2();
            vec2 position_old = vec2();
            vec2 acceleration = vec2();
            vec2 force = vec2();

            float theta = 0.0f; 
            float theta_old = 0.0f;
            float angular_acceleration = 0.0f;
            float torque_force = 0.0f;
            
            CollisionInfo last_collision;
    };
}