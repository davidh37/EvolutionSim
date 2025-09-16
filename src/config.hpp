#pragma once

#include "engine/common.hpp"

namespace config {
    // SIMULATION
    const string SIM_NAME = "Evo Sim v0.1";
    static constexpr int SIM_TICK_RATE = 60;
    static constexpr int SIM_MAX_ENTITIES = 8000;
    static constexpr int SIM_MAX_CREATURES = 1000;
    static constexpr float SIM_DELTA = 1.0f / SIM_TICK_RATE;
    static constexpr uint32_t SIM_SEED = 0;

    // PHYSICS
    static constexpr float PHYSICS_FRICTION = 350.0f;
    static constexpr float PHYSICS_ANGULAR_FRICTION = 100.0f;
    static constexpr float PHYSICS_COLLISION_FORCE = 0.08f;
    static constexpr float PHYSICS_MAX_RADIUS = 0.5;
    static constexpr int PHYSICS_MAP_WIDTH = 200;
    static constexpr vec2 PHYSICS_MAP_CENTER = vec2(PHYSICS_MAP_WIDTH * 0.5f, PHYSICS_MAP_WIDTH * 0.5f);
    static constexpr float PHYSICS_MAP_BROWNIAN_FORCE = 0.002f;
    static constexpr float PHYSICS_MAP_BROWNIAN_TORQUE = 0.0002f;
    static constexpr float PHYSICS_MAP_CENTER_GRAVITY = 0.000f;
    static constexpr int PHYSICS_MAP_UPDATE_RATE = 60;

    // CREATURES
    static constexpr int CREATURE_DNA_SIZE = 1024;
    static constexpr int CREATURE_MAX_APPENDAGES = 16;
    static constexpr int CREATURE_GENERATOR_SEED = 13371339;
    static constexpr float CREATURE_KLEIBER_CONSTANT = 0.5f;
    static constexpr float CREATURE_METABOLIC_RATE = 0.0025f;
    static constexpr float CREATURE_MUTATION_RATE = 0.01f;
    static constexpr float CREATURE_BIRTH_ENERGY = 0.4f;
    static constexpr float CREATURE_BIRTH_COST = 0.6f;
    static constexpr float CREATURE_MIN_ENERGY = 0.0f;
    static constexpr float CREATURE_MAX_ENERGY = 1.0f;
    static constexpr float CREATURE_FEEDING_RATE = 0.2f;
    static constexpr float CREATURE_MIN_DIGESTION = 0.2f;
    static constexpr float CREATURE_JET_FORCE = 80.0f; // divided by r²
    static constexpr float CREATURE_JET_COST = 0.06f;
    static constexpr float CREATURE_TURN_FORCE = 100.0f; // divided by r³
    static constexpr float CREATURE_TURN_COST = 0.02f;
    static constexpr float CREATURE_EYE_RANGE = 9.0f;
    static constexpr float CREATURE_EYE_COST = 0.00001f;
    static constexpr float CREATURE_THINK_COST = 0.000002f;
    static constexpr float CREATURE_SPIKE_FORCE = 3.5f;
    static constexpr float CREATURE_SPIKE_COST = 0.0001f;
    static constexpr int CREATURE_MOVE_COOLDOWN = 20;

    // PLANT
    static constexpr float PLANT_GROWTH = 0.00005f;
    static constexpr float PLANT_BIRTH_ENERGY = 0.05f;
    static constexpr float PLANT_MAX_ENERGY = 0.35f;

     // BRAIN
    static constexpr int BRAIN_SIZE = 26;
    static constexpr int BRAIN_SYNAPSE_RADIUS = 3;
    static constexpr int BRAIN_SYNAPSE_WIDTH = BRAIN_SYNAPSE_RADIUS * 2 + 1;
    static constexpr int BRAIN_FULL_SIZE = BRAIN_SIZE + 2 * BRAIN_SYNAPSE_RADIUS;
    static constexpr float BRAIN_ACTION_THRESHOLD = 1.0f;
    static constexpr float BRAIN_INPUTRATE_MIN = 0.0f;
    static constexpr float BRAIN_INPUTRATE_MAX = 1.0f;
    static constexpr float BRAIN_LEAKRATE_MIN = 0.2f;
    static constexpr float BRAIN_LEAKRATE_MAX = 8.0f;
    static constexpr float BRAIN_NEURON_INPUTRATE_MIN = 0.0f;
    static constexpr float BRAIN_NEURON_INPUTRATE_MAX = 1.0f;
    static constexpr float BRAIN_NEURON_LEAKRATE_MIN = 0.2f;
    static constexpr float BRAIN_NEURON_LEAKRATE_MAX = 8.0f;
    static constexpr float BRAIN_SYNAPSE_MIN = -3.0f;
    static constexpr float BRAIN_SYNAPSE_MAX = 3.0f;

    // RENDERING
    static constexpr int RENDER_EYE_SEGMENTS = 24;
    static constexpr int RENDER_CIRCLE_SEGMENTS = 32;
    static constexpr bool RENDER_RESIZABLE = true;
    static constexpr int RENDER_RESOLUTION_X = 1024;
    static constexpr int RENDER_RESOLUTION_Y = 768;

    // CAMERA
    static constexpr float CAM_X = 150.0f;
    static constexpr float CAM_Y = 150.0f;
    static constexpr float CAM_Z = -50.0f;
    static constexpr float CAM_ACCEL = 1000.0f;
    static constexpr float CAM_ZOOMCEL = 100.0f;
    static constexpr float CAM_DAMPING = 0.0005f;
    static constexpr float CAM_MIN_X = -50.0f;
    static constexpr float CAM_MAX_X = 350.0f;
    static constexpr float CAM_MIN_Y = -50.0f;
    static constexpr float CAM_MAX_Y = 350.0f;
    static constexpr float CAM_MIN_Z = -199.0f;
    static constexpr float CAM_MAX_Z = -0.15f;
    static constexpr float CAM_LERP = 5.0f;
}