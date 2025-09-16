#include "particles.hpp"
#include "ecs.hpp"
#include "config.hpp"
#include "creatures_physics_IO.hpp"

namespace particles {

    using namespace ecs;

    void initialize(){

    }

    void cleanup(){

    }

    void update(){
        for(CID cid = 0; cid < particle_data.vector.size(); cid++){

            ParticleData &particle = particle_data.vector[cid];
            CID cid2 = physics_bodies.cid_map[particle_data.id_map[cid]];
            PhysicsBody &body = physics_bodies.vector[cid2];

            assert(particle.dead == false);

            if(particle.energy < config::PLANT_MAX_ENERGY){
                particle.energy += config::PLANT_GROWTH;
            }
            
            if(body.last_collision.other_id != INVALID_CID){
                CID other_creature = creature_data.cid_map[physics_bodies.id_map[body.last_collision.other_id]];
                vec2 normal = -body.last_collision.other_vector;
                if(other_creature != INVALID_CID){
                    ecs::CreatureData Creature = creature_data.vector[other_creature];
                    if(Creature.state & CreatureData::ALIVE){
                        particle.energy -= creatures_physics_IO::getFeedingRate(Creature, normal, false) * config::SIM_DELTA;
                    }
                }
                body.last_collision.other_id = INVALID_CID;
            }

            if(particle.energy >= config::PLANT_MAX_ENERGY){
                particle.energy = config::PLANT_MAX_ENERGY;
            }
            if(particle.energy < 0.01f){
                particle.dead = true;
                particle.energy = 0.01f;
            }

            
        }
    }
}