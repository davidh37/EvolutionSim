#include "ecs.hpp"
#include "config.hpp"
#include "engine/mesh.hpp"
#include <array>

namespace ecs {

    ComponentVector<PhysicsBody> physics_bodies;
    ComponentVector<CreatureData> creature_data;
    ComponentVector<ParticleData> particle_data;

    std::array<Mesh, config::SIM_MAX_CREATURES> meshes;

    std::queue<ID> freeEntities;
    ID entitiesAlive = 0;
    CID cellsAlive = 0;

    void initialize(){
        for(ID id = 0; id < config::SIM_MAX_ENTITIES; id++){
            freeEntities.push(id);
        }
        physics_bodies.setCapacity(config::SIM_MAX_ENTITIES, config::SIM_MAX_ENTITIES);
        creature_data.setCapacity(config::SIM_MAX_CREATURES, config::SIM_MAX_ENTITIES);
        particle_data.setCapacity(config::SIM_MAX_ENTITIES, config::SIM_MAX_ENTITIES);
    }

    void cleanup(){
        for(int i = 0; i < config::SIM_MAX_CREATURES; i++){
            meshes[i].destroy();
        }
    }

    ID allocateCell(){
        // entity id management
        if(cellsAlive >= config::SIM_MAX_CREATURES){
            freeRandomCell();
        }
        if(entitiesAlive >= config::SIM_MAX_ENTITIES){
            freeRandomFood();
        }
        
        ID id = freeEntities.front();
        assert(id != INVALID_ID);
        freeEntities.pop();
        cellsAlive++;
        entitiesAlive++;

        // add components
        physics_bodies.add(id);
        creature_data.add(id);

        return id;
    }

    ID allocateFood(){
        if(entitiesAlive == config::SIM_MAX_ENTITIES){
            freeRandomFood();
        }

        ID id = freeEntities.front();
        freeEntities.pop();
        entitiesAlive++;

        physics_bodies.add(id);
        particle_data.add(id);

        return id;
    }

    void freeCell(ID id){
        // we trust that id is a cell
        physics_bodies.remove(id);
        creature_data.remove(id);

        freeEntities.push(id);
        cellsAlive--;
        entitiesAlive--;
    }

    void freeFood(ID id){
        // we trust that id is a food
        physics_bodies.remove(id);
        particle_data.remove(id);

        freeEntities.push(id);
        entitiesAlive--;
    }

    void freeRandomCell(){
        assert(creature_data.vector.size() > 0);
        CID r = rand() % (creature_data.vector.size());
        freeCell(creature_data.id_map[r]);
    }

    void freeRandomFood(){
        assert(particle_data.vector.size() > 0);
        CID r = rand() % (particle_data.vector.size());
        freeFood(particle_data.id_map[r]);
    }
}