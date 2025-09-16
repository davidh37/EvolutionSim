#include "engine/common.hpp"
#include "systems/physics.hpp"
#include "ecs.hpp"
#include "config.hpp"

#include <cmath>

namespace physics {

    static void integratePosition(ecs::PhysicsBody &b);
    static inline void solveCollisionPair(ecs::CID a, ecs::CID b);
    static void registerRegionMembers();
    static void solveCollisions();
    static void randomizeFlow(uint64 tick);

    void initialize(){
        randomizeFlow(0);
    }

    void cleanup(){

    }

    void update(uint64 tick){
        registerRegionMembers();
        solveCollisions();

        // integration
        for(size_t i = 0; i < ecs::physics_bodies.vector.size(); i++){
            integratePosition(ecs::physics_bodies.vector[i]);
        }

        static int counter = 0;
        counter++;
        if(counter % config::PHYSICS_MAP_UPDATE_RATE == 0){
            randomizeFlow(tick);
        }
    }

    /* BODY MANIPULATION HELPERS */

    static inline void integratePosition(ecs::PhysicsBody &b){
        assert(b.mass > 0.0f);

        // linear integration
        vec2 distance_center = config::PHYSICS_MAP_CENTER - b.position;
        b.force += b.mass * config::PHYSICS_MAP_CENTER_GRAVITY * distance_center;

        vec2 velocity = b.position - b.position_old;
        b.force += b.mass * b.friction * (-1.0f) * velocity;
        b.acceleration = b.force / b.mass;
        b.position_old = b.position;
        b.position =  b.position + velocity + b.acceleration * config::SIM_DELTA * config::SIM_DELTA;

        // rotational integration
        float angular_velocity = b.theta - b.theta_old;
        b.torque_force += b.mass * b.angular_friction * (-1.0f) * angular_velocity;
        b.angular_acceleration = b.torque_force / b.mass;
        b.theta_old = b.theta;
        b.theta = b.theta + angular_velocity + b.angular_acceleration * config::SIM_DELTA * config::SIM_DELTA;
        if(b.theta > 2.0f * PI){
            b.theta -= 2.0f * PI;
            b.theta_old -= 2.0f * PI;
        }else if(b.theta < 0.0f){
            b.theta += 2.0f * PI;
            b.theta_old += 2.0f * PI;
        }
        
        b.force = vec2(0.0f, 0.0f);
        b.acceleration = vec2(0.0f, 0.0f);
        b.torque_force = 0.0f;
        b.angular_acceleration = 0.0f;
    }

    static inline void solveCollisionPair(ecs::CID a, ecs::CID b){
        ecs::PhysicsBody &A = ecs::physics_bodies.vector[a];
        ecs::PhysicsBody &B = ecs::physics_bodies.vector[b];

        vec2 collision_axis = A.position - B.position;
        float dist = glm::length(collision_axis);
        if(dist < A.radius + B.radius){
            if(dist == 0.0f){
                //cout << "[physics] WARNING! Two physics_bodies had an identical position, this shouldn't happen!" << endl;
                collision_axis = B.position * 0.0001f;
                dist = 0.001f;
            }

            const vec2 normal = collision_axis / dist;
            const float delta = A.radius + B.radius - dist;

            //mass is proportional to r² or r³ ?
            vec2 force = delta * normal / (A.mass + B.mass);
            // only move fraction because it's less buggy collisions!
            A.position += config::PHYSICS_COLLISION_FORCE * B.mass * force;
            B.position -= config::PHYSICS_COLLISION_FORCE * A.mass * force;

            A.last_collision.other_id = b;
            B.last_collision.other_id = a;
            A.last_collision.other_vector = -normal;
            B.last_collision.other_vector = normal;
        }
    }



    /* REGION LOGIC */

    struct Region {
        std::vector<ecs::CID> members;
        vec2 flow;
    };


    static Region regions[config::PHYSICS_MAP_WIDTH][config::PHYSICS_MAP_WIDTH];
    
    static inline float i2f(uint32 x){
        return 2.0f * (float)x / (float)UINT32_MAX - 1.0f;
    }

    static void randomizeFlow(uint64 tick){
        static int y = 0;
        for(int x = 0; x < config::PHYSICS_MAP_WIDTH; x++){
            uint32 r = hash(tick * 2 * config::PHYSICS_MAP_WIDTH + 2 * x + config::SIM_SEED);
            uint32 r2 = hash(tick * 2 * config::PHYSICS_MAP_WIDTH + 2 * x + config::SIM_SEED + 1);
            
            regions[y][x].flow = vec2(i2f(r), i2f(r2));
        }
        y = (y + 1) % config::PHYSICS_MAP_WIDTH;
    }

    ecs::CID findBody(vec2 position){
        int cell_x = (int)position.x;
        int cell_y = (int)position.y;

        if(cell_x < 0 || cell_y < 0 || cell_x >= config::PHYSICS_MAP_WIDTH || cell_y >= config::PHYSICS_MAP_WIDTH){
            return ecs::INVALID_CID;
        }

        //cout << "findPhysicsBody() in " << cell_x << " " << cell_y << endl;

        std::vector<ecs::CID> candidates = regions[cell_y][cell_x].members;
        for(size_t i = 0; i < candidates.size(); i++){
            ecs::PhysicsBody &A = ecs::physics_bodies.vector[candidates[i]];
            vec2 dist = position - A.position;
            if(dist.x * dist.x + dist.y * dist.y < A.radius * A.radius){
                return candidates[i];
            }
        }
        return ecs::INVALID_CID;
    }

    static void registerRegionMembers(){
        // erase all previous storage
        for(int y = 0; y < config::PHYSICS_MAP_WIDTH; y++){
            for(int x = 0; x < config::PHYSICS_MAP_WIDTH; x++){
                regions[y][x].members.clear();
            }
        }

        // register new locations
        int n = ecs::physics_bodies.vector.size();
        for(int b = 0; b < n; b++){
            ecs::PhysicsBody &body = ecs::physics_bodies.vector[b];

            int cell_x = (int)body.position.x;
            int cell_y = (int)body.position.y;
            float remainder_x = body.position.x - cell_x;
            float remainder_y = body.position.y - cell_y;
            int offset_x = remainder_x < 0.5f ? -1.0f : 1.0f;
            int offset_y = remainder_y < 0.5f ? -1.0f : 1.0f;

            if(cell_x < 0 || cell_y < 0 || cell_x >= config::PHYSICS_MAP_WIDTH || cell_y >= config::PHYSICS_MAP_WIDTH){
                // outside of collision detection
                continue;
            }
         
            bool valid_x = cell_x + offset_x >= 0 && cell_x + offset_x < config::PHYSICS_MAP_WIDTH;
            bool valid_y = cell_y + offset_y >= 0 && cell_y + offset_y < config::PHYSICS_MAP_WIDTH;

            regions[cell_y][cell_x].members.push_back(b);
            if(valid_x){
                regions[cell_y][cell_x + offset_x].members.push_back(b);
            }
            if(valid_y){
                regions[cell_y + offset_y][cell_x].members.push_back(b);
            }
            if(valid_x && valid_y){
                regions[cell_y + offset_y][cell_x + offset_x].members.push_back(b);
            }   
        }
    }

    static void solveCollisions(){
        for(int y = 0; y < config::PHYSICS_MAP_WIDTH; y++){
            for(int x = 0; x < config::PHYSICS_MAP_WIDTH; x++){
                std::vector<ecs::CID> &candidates = regions[y][x].members;
                int n = candidates.size();

                for(int i = 0; i < n; i++){
                    // small brownian acceleration
                    ecs::PhysicsBody &A = ecs::physics_bodies.vector[candidates[i]];
                    A.force += config::PHYSICS_MAP_BROWNIAN_FORCE * regions[y][x].flow;
                    A.torque_force += config::PHYSICS_MAP_BROWNIAN_TORQUE * regions[y][x].flow.x;

                    for(int j = i+1; j < n; j++){
                        solveCollisionPair(candidates[i], candidates[j]);
                    }
                }
            }
        }
    }

    





    static void raycast_cell(RaycastInfo &info, Region &region, vec2 normal, vec2 start_position){
       float min = (float)1e20;
       for(int i = 0; i < (int)region.members.size(); i++){

            ecs::PhysicsBody &target = ecs::physics_bodies.vector[region.members[i]];
            vec2 to_target = target.position - start_position;
            float ray_scale = glm::dot(normal, to_target);
            if(ray_scale <= 0.0f){
                // target is behind
                continue;
            }
            vec2 hit = normal * ray_scale;
            vec2 in_circle = start_position + hit - target.position;
            
            if(glm::length2(in_circle) < target.radius * target.radius){
                float len = glm::length2(to_target);
                if(len < min){
                    info.hit_id = region.members[i];
                    info.distanceSq = glm::length2(to_target);
                    min = len;
                }
            }
        }
    }

   

    RaycastInfo raycast(vec2 position, vec2 normal, float range){

        vec2 start_position = position;
        float distanceSq = 0.0f;
        RaycastInfo info;
        info.hit_id = ecs::INVALID_CID;

        // sign of ray steps
        int step_x = normal.x < 0 ? -1 : 1;
        int step_y = normal.y < 0 ? -1 : 1;
        // scale of ray steps in each dimension to reach a distance of 1
        float ndx = (normal.x == 0.0f) ? 1e20 : 1 / normal.x;
        float ndy = (normal.y == 0.0f) ? 1e20 : 1 / normal.y;

        while(distanceSq < range * range){
            
            int cell_x = (int)position.x;
            int cell_y = (int)position.y;

            if(cell_x < 0 || cell_y < 0 || cell_x >= config::PHYSICS_MAP_WIDTH || cell_y >= config::PHYSICS_MAP_WIDTH){
                break;
            }

            raycast_cell(info, regions[cell_y][cell_x], normal, start_position);

            if(info.hit_id != ecs::INVALID_CID){
                info.distanceSq = std::min(range * range, info.distanceSq);
                return info;
            }

            int next_cell_x = cell_x + step_x;
            int next_cell_y = cell_y + step_y;

            float scale_x = abs((next_cell_x - position.x) * ndx);
            float scale_y = abs((next_cell_y - position.y) * ndy);
            
            if(scale_x < scale_y){
                position.x = (float)next_cell_x;
                position.y = position.y + scale_x * normal.y;
            }else{
                position.x = position.x + scale_y * normal.x;
                position.y = (float)next_cell_y;
            }
            distanceSq = glm::length2(position - start_position);
        }

        info.hit_id = ecs::INVALID_CID;
        info.distanceSq = range * range;
        return info;
    }
}

