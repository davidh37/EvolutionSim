#include "systems/environment.hpp"
#include "ecs.hpp"
#include "physics.hpp"
#include "util/markov_name.hpp"

namespace environment {

    using namespace ecs;

    static float growth_rate = 5000.0f;

    static void spawnCreature(vec2 position);
    static ID reproduceCreature(ID creature);

    void initialize(){
        markov_name::initialize("resources/species.txt");

        for(int i = 0; i < 1000; i++){
            float min_x = config::PHYSICS_MAP_WIDTH * 0.2f;
            float max_x = config::PHYSICS_MAP_WIDTH * 0.8f;
            spawnCreature(vec2(randf(min_x, max_x), randf(min_x, max_x)));
        }

        /*
		for (int i = 0; i < 8000; i++) {
			float min_x = config::PHYSICS_MAP_WIDTH * 0.2f;
			float max_x = config::PHYSICS_MAP_WIDTH * 0.8f;
			spawnFood(vec2(randf(min_x, max_x), randf(min_x, max_x)));
		}*/

        cout << sizeof(CreatureData) << endl;
        cout << sizeof(ParticleData) << endl;
        cout << sizeof(PhysicsBody) << endl;
    }

    void cleanup(){

    }

    void update(uint64 tick){

        while(entitiesAlive - cellsAlive < growth_rate){
            float minval = 0.2f * config::PHYSICS_MAP_WIDTH;
            float maxval = 0.8f * config::PHYSICS_MAP_WIDTH;
            spawnFood(vec2(randf(minval, maxval), randf(minval, maxval)));
        }
        
        static std::vector<ID> reproduce_creatures;
        static std::vector<ID> kill_creatures;
        static std::vector<ID> kill_foods;
        kill_creatures.clear();
        kill_foods.clear();
        reproduce_creatures.clear();

        for(CID cid = 0; cid < particle_data.vector.size(); cid++){
            ID id = particle_data.id_map[cid];
            CID body_cid = physics_bodies.cid_map[id];
            PhysicsBody &body = physics_bodies.vector[body_cid];
            assert(particle_data.vector[cid].energy > 0.0f);
            body.radius = 0.5f * sqrt(particle_data.vector[cid].energy / config::PLANT_MAX_ENERGY);
            body.mass = body.radius * body.radius * 20;
            if(particle_data.vector[cid].dead){
                kill_foods.push_back(particle_data.id_map[cid]);
            }
        }

        int plant_surplus = entitiesAlive - cellsAlive - growth_rate;
        for(int i = 0; i < plant_surplus; i++){
            CID r = rand() % (particle_data.vector.size());
			kill_foods.push_back(particle_data.id_map[r]);
        }
        
        for(CID cid = 0; cid < creature_data.vector.size(); cid++){
            ID id = creature_data.id_map[cid];
            CreatureData &creature = creature_data.vector[cid];

            switch(creature.state){
                case CreatureData::FETUS:
                    // wait for generator
                    break;
                case CreatureData::ALIVE:
                    // wait til birth or dead
                    break;
                case CreatureData::READY:
                    creature.state = CreatureData::ALIVE;
                    physics_bodies.vector[physics_bodies.cid_map[id]].radius = 0.5f * creature.size;
                    physics_bodies.vector[physics_bodies.cid_map[id]].mass = 0.25f * creature.size * creature.size;
                    creature.energy = creature.size * creature.size * config::CREATURE_BIRTH_ENERGY;
                    break;
                case CreatureData::BIRTH:
                    // spawn offspring
                    creature.state = CreatureData::ALIVE; // order matters
                    reproduce_creatures.push_back(id);
                    break;
                case CreatureData::DEAD:
                    // despawn
                    kill_creatures.push_back(id);
                    break;
                default:
                    assert(false);
            }
        }

        std::vector<ID> skip_these;
        for(ID id : reproduce_creatures){
            bool skip = false;
            for(ID id2 : skip_these){
                if(id == id2){
                    // original birthing creature has been killed
                    skip = true;
                }
			}
            if(!skip) {
                ID ret = reproduceCreature(id);
                skip_these.push_back(ret);
            }
        }
        for(ID id : kill_creatures){
            freeCell(id);
        }
        for(ID id : kill_foods){
            freeFood(id);
        }
    }

    void addGrowthRate(float rate_delta){
        growth_rate += rate_delta;
        if (growth_rate < 0.0f) {
            growth_rate = 0.0f;
        }
        if (growth_rate > config::SIM_MAX_ENTITIES - config::SIM_MAX_CREATURES) {
			growth_rate = config::SIM_MAX_ENTITIES - config::SIM_MAX_CREATURES;
        }
    }

    float getGrowthRate(){
        return growth_rate;
    }

    static int reserveCreatureMesh(CID creature_cid){
        // find free mesh
        bool taken_meshes[config::SIM_MAX_CREATURES] = {false};
        for(CID cid = 0; cid < creature_data.vector.size(); cid++){
            if(cid != creature_cid){
                // avoid marking own default value 0
                taken_meshes[creature_data.vector[cid].mesh_id] = true;
            }
        }
        int index = -1;
        for(int i = 0; i < config::SIM_MAX_CREATURES; i++){
            if(!taken_meshes[i]){
                index = i;
            }
        }
        assert(index != -1);
        return index;
    }

    static ID reproduceCreature(ID creature_id){
        CID creature_cid = creature_data.cid_map[creature_id];
        CID body_cid = physics_bodies.cid_map[creature_id];
        CreatureData &creature = creature_data.vector[creature_cid];
        PhysicsBody& body = physics_bodies.vector[body_cid];

		if (creature.state != CreatureData::ALIVE) {
			cout << "OH NO reproduction fail :D" << endl;
			return INVALID_ID;
		}

        // set up child dna + position
        vec2 birth_position = body.position + randv() * body.radius * 1.1f;
        ubyte birth_dna[config::CREATURE_DNA_SIZE];
        memcpy(birth_dna, creature.dna, config::CREATURE_DNA_SIZE);
        int mutation_count = (int) ((float)config::CREATURE_DNA_SIZE * config::CREATURE_MUTATION_RATE * creature.mutation_rate);
        int generation_parent = creature.generations;
        int mutations_parent = creature.mutations;
        string name_parent = creature.name;
        for(int i = 0; i < mutation_count; i++){
            uint32 byte_index = rand() % config::CREATURE_DNA_SIZE;
            uint32 bit_index = rand() % 8;
            ubyte mutation = 1 << bit_index;
            birth_dna[byte_index] ^= mutation;
        }

        // spawn child
        ID id = allocateCell(); // todo terrible bug if this messes up the ID order by removing alive cell
        PhysicsBody &body2 = physics_bodies.vector[physics_bodies.cid_map[id]];
        CreatureData &creature2 = creature_data.vector[creature_data.cid_map[id]];
        memcpy(creature2.dna, birth_dna, config::CREATURE_DNA_SIZE);
        creature2.name = name_parent;
        if (rand() % 100 == 69) {
            markov_name::mutateWord(creature2.name);
        }
        creature2.mutations = mutations_parent + mutation_count;
        creature2.generations = generation_parent + 1;
        body2.position = birth_position;
        body2.position_old = birth_position;
        creature2.mesh_id = reserveCreatureMesh(creature_data.cid_map[id]);
        creature2.state = CreatureData::FETUS;

        return id;
    }

    static void spawnCreature(vec2 position){
        ID id = allocateCell();

        PhysicsBody &body = physics_bodies.vector[physics_bodies.cid_map[id]];
        CreatureData &creature = creature_data.vector[creature_data.cid_map[id]];
        for(int i = 0; i < config::CREATURE_DNA_SIZE; i++){
            creature.dna[i] = rand();
        }
        creature.name = markov_name::generateWord(4, 16);
        body.position = position;
        body.position_old = position;
        creature.mesh_id = reserveCreatureMesh(creature_data.cid_map[id]);
    }

    void spawnFood(vec2 position){
        ID id = allocateFood();

        //ParticleData &particle = particle_data.vector[particle_data.cid_map[id]];
        PhysicsBody &body = physics_bodies.vector[physics_bodies.cid_map[id]];

        body.position = position;
        body.position_old = position;
    }

    
}