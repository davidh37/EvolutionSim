#pragma once
#include "engine/mesh.hpp"
#include "config.hpp"



namespace ecs {

    struct Appendage {
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
        int cooldown = 0;
    };

    struct Neuron {
        float potential = 0.5f;
        float input = 0.0f;

        // genetic
        enum Type {
            NORMAL,
            INPUT,
            OUTPUT 
        } type;
        float input_rate = 0.0f;
        float leak_rate = 0.0f;
        float synapses[config::BRAIN_SYNAPSE_WIDTH][config::BRAIN_SYNAPSE_WIDTH] = {0.0f};
    };

    struct CreatureData {
        int mutations = 0;
        int generations = 0;
        string name = "";

        ubyte dna[config::CREATURE_DNA_SIZE];
        //ubyte foreign_dna[config::CREATURE_DNA_SIZE];
        
        // phenotype (+ brain state)
        Appendage appendages[config::CREATURE_MAX_APPENDAGES];
        int appendage_count = 0;
        Neuron neurons[config::BRAIN_FULL_SIZE][config::BRAIN_FULL_SIZE];
        float size = 0.0f;     
        float metabolic_rate = 0.0f;                // efficacy test
        vec3 color = vec3();                        // rgb values
        float brain_input_rate = 0.0f;
        float brain_leak_rate = 0.0f;
        float carnivore = 0.0f;                     // at 1.0f best meat digestion, at 0.0f best plant
        // todo
        float sex = 0.0f;                           // 0 no sex, 1 dna from 100% closest species member                    
        float mutation_rate = 0.0f;                 // modify mutation chance
        

        // state
        enum State {
            FETUS = 0,
            ALIVE = 1,
            READY = 2,
            BIRTH = 3,
            DEAD = 4
        };
        State state = FETUS;
        float energy = 0;       
        int number_neurons_firing = 0;
		int feeding = 0;
        
        //int mutations = 0;
        //int generations_min = 1;
        //int generations_max = 1;
        //int kills = 0;
        //int offspring_mothered = 0;
        //int offspring_fathered = 0;
        //uint64_t life_time = 0;

        // graphics
        int mesh_id;
        bool highlighted = false;
        bool to_mesh = true;
        bool ui_source = false;
    };
}