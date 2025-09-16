#pragma once
#include "engine/common.hpp"
#include "engine/mesh.hpp"
#include <array>

namespace ecs {
    using ID = size_t;
    using CID = size_t;

    static constexpr ID INVALID_ID = SIZE_MAX;
    static constexpr CID INVALID_CID = SIZE_MAX;
}

#include "components/physics_body.hpp"
#include "components/creature_data.hpp"
#include "components/particle_data.hpp"

namespace ecs {

    template <class T>
    class ComponentVector {
        public:
            std::vector<T> vector; 
            std::vector<ID> id_map;
            std::vector<CID> cid_map;
            ID size_max = 0;

            void setCapacity(CID max_size, ID max_entities){
                cid_map.resize(max_entities);
                for(ID id = 0; id < max_entities; id++){
                    cid_map[id] = INVALID_CID;
                }
                vector.reserve(max_size);
                id_map.reserve(max_size);
                size_max = max_size;
            }

            void add(ID entity){
                // same size vectors
                assert(id_map.size() == vector.size());
                assert(vector.size()+1 <= size_max);
                vector.resize(vector.size()+1);
                id_map.resize(id_map.size()+1);
                id_map[id_map.size()-1] = entity;
                cid_map[entity] = vector.size()-1;
            }

            void remove(ID entity){
                CID index = cid_map[entity];
                assert(id_map.size() == vector.size());
                assert(index != INVALID_ID);
                assert(index < vector.size());
                assert(index >= 0);

                // copy back
                vector[index] = vector[vector.size()-1]; // todo exception out of bounds access
                id_map[index] = id_map[id_map.size()-1];
                cid_map[id_map[index]] = index;

                // shorten vector
                vector.resize(vector.size()-1);
                id_map.resize(id_map.size()-1);
                cid_map[entity] = INVALID_ID;
            }
    };

    extern ComponentVector<PhysicsBody> physics_bodies;
    extern ComponentVector<CreatureData> creature_data;
    extern ComponentVector<ParticleData> particle_data;

    extern std::array<Mesh, config::SIM_MAX_CREATURES> meshes;

    extern ID entitiesAlive;
    extern CID cellsAlive;

    void initialize();

    void cleanup();

    ID allocateCell();

    ID allocateFood();

    void freeCell(ID id);

    void freeRandomCell();

    void freeFood(ID id);

    void freeRandomFood();
}

