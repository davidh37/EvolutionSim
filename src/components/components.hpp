#pragma once
#include "engine/mesh.hpp"
#include "config.hpp"

/*

namespace ecs {

    struct CollisionBody {
        ID id;
        ID hid_id;
        vec2 position;
        float radius;
        float angle;
    }   

    struct RigidBody {
            // physical "constants"
            float radius = 0.2f;
            vec2 position
            vec2 position_old
            vec2 force
            float theta
            float theta_old
            float torque

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

    struct ParticleData {
        bool dead = false;
        float energy = 0.01f;
        vec3 color = COLOR_GREEN;
    };
    struct NeuronState {
        float potential = 0.5f;
        float input = 0.0f;
    };

    struct BrainState {
        Neuron neurons[config::BRAIN_FULL_SIZE][config::BRAIN_FULL_SIZE];
    };

    struct Neuron {
        enum Type {
            NORMAL,
            INPUT,
            OUTPUT 
        } type;
        float input_rate = 0.0f;
        float leak_rate = 0.0f;
        float synapses[config::BRAIN_SYNAPSE_WIDTH][config::BRAIN_SYNAPSE_WIDTH] = {0.0f};
    };

    struct Brain {
        Neuron neurons[config::BRAIN_SIZE][config::BRAIN_SIZE];
        float input_rate;
        float leak_rate;
    };

    struct Part {
        enum Type {
            SPIKE,
            EYE,
            SKIN,
            NONE,
            JET,
            TURNER_LEFT,
            TURNER_RIGHT,
            TYPE_TOTAL
        };
        Type type = NONE;
        float angle = 0.0f;
        float strength = 0.0f;
        int neuron_x = 0;
        int neuron_y = 0;
    };

    struct CreatureStats {
        float average_part_cost;
        float effective_energy;          
        int mutations = 0;
        int generations = 1;
        int kills = 0;
        int offspring = 0;
    };

    struct Genome {
        ubyte dna[config::CREATURE_DNA_SIZE];

        bool constructed;

        Brain brain;
        Part parts[config::CREATURE_MAX_PARTS];
        vec3 color = vec3();                        // rgb values
        float size = 0.0f;     
        float metabolic_rate = 0.0f;                // efficacy test
        float carnivore = 0.0f;                     // at 1.0f best meat digestion, at 0.0f best plant
        float mutation_rate = 0.0f; 
        float sex = 0.0f; 
    };

    struct Creature {
        Genome genome;
        BrainState brain_state;
        bool dead;
        float energy;
        int number_neurons_firing; 
        int mesh_id;

        CreatureStats stats;
    }

        struct CollisionInfo {
        CID other_id = INVALID_CID;              // physics body
        vec2 other_vector = vec2();
    };

    
}*/