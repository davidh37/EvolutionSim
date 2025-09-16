#include "creatures_physics_IO.hpp"
#include "ecs.hpp"
#include "systems/physics.hpp"
#include "util/debuglines.hpp"

namespace creatures_physics_IO {

    using namespace ecs;

    static float handleAppendages(CID cid_creature, CID cid_body);

    void initialize(){

    }

    void cleanup(){

    }

    float getFeedingRate(CreatureData &creature, vec2 normal, bool meat){
        float angle = (atan2f(normal.y, normal.x) / (2.0f * PI)) + 0.5f;
        angle = CLAMP(angle, 0.0f, 0.999f);
        int appendage_index = (int)(angle * (float)creature.appendage_count);
        assert(appendage_index >= 0);
        assert(appendage_index < creature.appendage_count);
        

        float min_factor = config::CREATURE_MIN_DIGESTION;
        float feed_rate = config::CREATURE_FEEDING_RATE;
        if(meat){
            feed_rate *= (min_factor + (1.0f - min_factor) * creature.carnivore);
        }else{
            feed_rate *= (min_factor + (1.0f - min_factor) * (1.0f - creature.carnivore));
        }
        feed_rate *= creature.size;
        
        Appendage &app = creature.appendages[appendage_index];
        if(app.type == Appendage::SPIKE){
            return feed_rate * config::CREATURE_SPIKE_FORCE * app.strength;
        }
        return feed_rate;
    }

    float getMetabolicRate(CreatureData &creature, float appendage_cost){
        float metabolism = config::CREATURE_METABOLIC_RATE * (1.0f + creature.metabolic_rate);
        float energy_cost = metabolism * powf(creature.size * creature.size, config::CREATURE_KLEIBER_CONSTANT);
        energy_cost += creature.size * config::CREATURE_THINK_COST * creature.number_neurons_firing;
        energy_cost += creature.size * appendage_cost;
        return energy_cost;
    }

    void update(){
        
        for(CID cid = 0; cid < creature_data.vector.size(); cid++){
            CreatureData &creature = creature_data.vector[cid];
            ID cid2 = physics_bodies.cid_map[creature_data.id_map[cid]];
            PhysicsBody &body = physics_bodies.vector[cid2];

            if(creature.state & CreatureData::ALIVE){
                float appendage_cost = handleAppendages(cid, cid2);
                creature.energy -= getMetabolicRate(creature, appendage_cost) * config::SIM_DELTA;

                // collision code
                if(body.last_collision.other_id != INVALID_CID){
                    ID other_id = physics_bodies.id_map[body.last_collision.other_id];
                    CID other_creature_cid = creature_data.cid_map[other_id];
                    CID other_particle_cid = particle_data.cid_map[other_id];
                    
					creature.feeding = 0;
                    if(other_creature_cid != INVALID_CID){
                        
						CreatureData& other_creature = creature_data.vector[other_creature_cid];
                        if (creature.state & CreatureData::ALIVE && other_creature.state & CreatureData::ALIVE) {

                            float our_feeding_rate = getFeedingRate(creature, body.last_collision.other_vector, true);

                            other_creature.energy -= our_feeding_rate * config::SIM_DELTA;
                            creature.energy += our_feeding_rate * config::SIM_DELTA;
                            creature.feeding = 2;
                        }
                    }else if(other_particle_cid != INVALID_CID){
                        float our_feeding_rate = getFeedingRate(creature, body.last_collision.other_vector, false);
                        creature.energy += our_feeding_rate * config::SIM_DELTA;
						creature.feeding = 1;
                    }
                    assert(other_particle_cid != INVALID_CID || other_creature_cid != INVALID_CID);
                    body.last_collision.other_id = INVALID_CID;
                }
                
                if(creature.energy < config::CREATURE_MIN_ENERGY * creature.size * creature.size){
                    creature.state = CreatureData::DEAD;
                }
                if(creature.energy >= config::CREATURE_MAX_ENERGY * creature.size * creature.size && creature.state != CreatureData::BIRTH){
                    creature.state = CreatureData::BIRTH;
                    creature.energy -= config::CREATURE_BIRTH_COST * creature.size * creature.size;
                }
            }
        }
    }

    static float handleAppendages(CID cid_creature, CID cid_body){
        CreatureData &creature = creature_data.vector[cid_creature];
        PhysicsBody &body = physics_bodies.vector[cid_body];


        // sensors
		const float sensor1 = body.position.x / config::PHYSICS_MAP_WIDTH;
		const float sensor2 = body.position.y / config::PHYSICS_MAP_WIDTH;
		const float d = config::PHYSICS_MAP_WIDTH;
		const float sensor3 = glm::distance2(body.position, config::PHYSICS_MAP_CENTER) / (d * d);
		const float sensor4 = sinf(body.theta);
		const float sensor5 = cosf(body.theta);
		

		creature.neurons[15][10].potential += 0.25f * sensor1;
		creature.neurons[15][12].potential += 0.25f * sensor2;
		creature.neurons[15][14].potential += 0.25f * sensor3;
		creature.neurons[15][16].potential += 0.5f * sensor4;
		creature.neurons[15][18].potential += 0.5f * sensor5;
		creature.neurons[17][10].potential += 0.25f * creature.energy;
		creature.neurons[17][12].potential += (creature.feeding == 1 ? 0.5f : 0.0f);
		creature.neurons[17][14].potential += (creature.feeding == 2 ? 0.5f : 0.0f);
       
		

        float total_cost = 0.0f;
        for(int i = 0; i < config::CREATURE_MAX_APPENDAGES; i++){
            Appendage &appendage = creature.appendages[i];
            float angle = body.theta + appendage.angle;
            float ACTION = config::BRAIN_ACTION_THRESHOLD;
            vec2 normal = vec2(cos(angle), sin(angle));

            assert(appendage.strength >= 0.0f && appendage.strength <= 1.0f);


            switch(appendage.type){
                case Appendage::SPIKE:
                    total_cost += config::CREATURE_SPIKE_COST * appendage.strength;
                    break;
                case Appendage::EYE:
                    {
                    // TODO
                    float strength = config::CREATURE_EYE_RANGE * appendage.strength * creature.size;
                    vec2 origin = body.position + normal * body.radius * 1.05f;
                    physics::RaycastInfo info = physics::raycast(origin, normal, strength);
                    if(info.hit_id != INVALID_CID){
                        assert(info.hit_id != cid_body); // don't hit ourselves lol
						ID id = ecs::physics_bodies.id_map[info.hit_id];
						assert(id != INVALID_ID);
						CID cidc = ecs::creature_data.cid_map[id];
						CID cidp = ecs::particle_data.cid_map[id];
						if (cidc != INVALID_CID) {
							creature.neurons[appendage.neuron_y][appendage.neuron_x].potential += 0.5f;
						}
						else if (cidp != INVALID_CID) {
							creature.neurons[appendage.neuron_y][appendage.neuron_x+1].potential += 0.5f;
						}
						creature.neurons[appendage.neuron_y + 1][appendage.neuron_x - 1].potential += ecs::physics_bodies.vector[info.hit_id].radius;
                    }
                    bool hit = info.hit_id != INVALID_CID;
                    
                    if(creature.highlighted){
                        if(info.distanceSq > 0.0f){
                            debuglines::addPoint(origin, hit ? COLOR_GREEN : COLOR_RED);
                            debuglines::addPoint(origin + normal * sqrtf(info.distanceSq), hit ? COLOR_GREEN : COLOR_RED);
                        }
                    }
                    
                    total_cost += config::CREATURE_EYE_COST * appendage.strength;
                    }
                    break;
                case Appendage::SKIN:
                    break;
                case Appendage::NONE:
                    break;
                case Appendage::JET:
                    if(creature.neurons[appendage.neuron_y][appendage.neuron_x].potential >= ACTION){
                        assert(appendage.cooldown >= 0);
                        
                        if(appendage.cooldown == 0){
                            total_cost += config::CREATURE_JET_COST * appendage.strength * appendage.strength;
                            body.force -= config::CREATURE_JET_FORCE * normal * appendage.strength * creature.size * creature.size;
                            appendage.cooldown = config::CREATURE_MOVE_COOLDOWN;
                        }else{
                            appendage.cooldown--;
                        }
                    }
                    break;
                case Appendage::TURNER_LEFT:
                    if(creature.neurons[appendage.neuron_y][appendage.neuron_x].potential >= ACTION){
                        assert(appendage.cooldown >= 0);
                        
                        if(appendage.cooldown == 0){
                            total_cost += config::CREATURE_TURN_COST * appendage.strength * appendage.strength;
                            body.torque_force += config::CREATURE_TURN_FORCE * appendage.strength * creature.size * creature.size;
                            appendage.cooldown = config::CREATURE_MOVE_COOLDOWN;
                        }else{
                            appendage.cooldown--;
                        }
                    }
                    break;
                case Appendage::TURNER_RIGHT:
                    if(creature.neurons[appendage.neuron_y][appendage.neuron_x].potential >= ACTION){
                        assert(appendage.cooldown >= 0);
                        

                        if(appendage.cooldown == 0){
                            total_cost += config::CREATURE_TURN_COST * appendage.strength;
                            body.torque_force -= config::CREATURE_TURN_FORCE * appendage.strength * creature.size;
                            appendage.cooldown = config::CREATURE_MOVE_COOLDOWN;
                        }else{
                            appendage.cooldown--;
                        }
                    }
                    break;
                default:
                    abort();
            }
        }
        return total_cost;
    }
    
}