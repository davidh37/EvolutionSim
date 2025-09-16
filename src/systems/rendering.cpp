#include "systems/rendering.hpp"
#include "engine/common.hpp"
#include "engine/shader.hpp"
#include "engine/engine.hpp"
#include "engine/camera.hpp"
#include "engine/mesh.hpp"
#include "ecs.hpp"
#include "config.hpp"
#include "util/mesher_primitive.hpp"
#include "util/debuglines.hpp"
#include "util/gui.hpp"
#include "environment.hpp"


namespace rendering {

    static Mesh circle;
    static Shader default_shader;
    static Shader default_instance_shader;

    static int UI_state = 0;
    static string UI_sim_state_info = "-";
    static vec3 cam_position = vec3(config::CAM_X, config::CAM_Y, config::CAM_Z);
    static vec3 cam_velocity = vec3(0.0f, 0.0f, 0.0f);
    static vec3 cam_input_vector = vec3(0.0f, 0.0f, 0.0f);

    static void drawGui(ecs::CID UI_source);
    static void drawEntities();
    static void updateCamera(float dt, ecs::CID follow_target);

    void initialize(){
        engine::createGLWindow(config::SIM_NAME, config::RENDER_RESOLUTION_X, config::RENDER_RESOLUTION_Y, config::RENDER_RESIZABLE, 3, 3);
        debuglines::initialize();
        gui::initialize();

        resize(config::RENDER_RESOLUTION_X, config::RENDER_RESOLUTION_Y);   // fixviewport
        camera::setProjection(100.0f, 0.1f, 200.0f);
        camera::setPosition(cam_position);
        default_shader.compile(string("resources/color.vert"), string("resources/color.frag"));
        default_instance_shader.compile(string("resources/color_instance.vert"), string("resources/color.frag"));
    
        // create circle mesh
        Mesh::VertexBuffer vb;
        Mesh::IndexBuffer ib;
        mesher_primitive::circle(circle.vertex_buffer, circle.index_buffer, vec3(), vec2(1.0f), COLOR_GREEN, 32);
        circle.upload(Mesh::STATIC, true);
    }

    void cleanup(){
        default_shader.destroy();
        default_instance_shader.destroy();
    
        debuglines::cleanup();
        gui::cleanup();
    }

    void update(float real_delta){
        ecs::CID follow_target = ecs::INVALID_CID;
        ecs::CID UI_source = ecs::INVALID_CID;
        for(ecs::CID cid = 0; cid < ecs::creature_data.vector.size(); cid++){
            if(ecs::creature_data.vector[cid].highlighted){
                follow_target = cid;
            }
            if(ecs::creature_data.vector[cid].ui_source){
                UI_source = cid;
            }
        }

        updateCamera(real_delta, follow_target);

        engine::clearScreen(0.5, 0.5, 0.5);
        drawEntities();
        debuglines::render(camera::getProjectionMatrix() * camera::getViewMatrix(), 4.0f);
        drawGui(UI_source);
        engine::swapBuffer();
    }

    void resize(int x, int y){
        camera::setScreenSize(x, y);
        engine::setViewport(0, 0, x, y);
    }

    
    static void updateCamera(float dt, ecs::CID follow_target){

        float zoom_factor = 0.05f + (cam_position.z - config::CAM_MAX_Z) / (config::CAM_MIN_Z - config::CAM_MAX_Z);
        vec3 accel = cam_input_vector;
        accel.x *= zoom_factor * config::CAM_ACCEL;
        accel.y *= zoom_factor * config::CAM_ACCEL;
        accel.z *= zoom_factor * config::CAM_ZOOMCEL;
        
        
        vec2 old_pos = vec2(cam_position.x, cam_position.y);
        cam_velocity.x += accel.x * dt;
        cam_velocity.y += accel.y * dt;
        cam_velocity.z += accel.z;              // dt ruins scroll wheel   
        cam_position += cam_velocity * dt;
        cam_velocity *= powf(config::CAM_DAMPING, dt);

        if(follow_target != ecs::INVALID_CID){
            // lerp to target
            ecs::CID body_cid = ecs::physics_bodies.cid_map[ecs::creature_data.id_map[follow_target]];
            vec2 target_pos = ecs::physics_bodies.vector[body_cid].position;
            old_pos = glm::mix(old_pos, target_pos, config::CAM_LERP * dt);
            cam_position = vec3(old_pos.x, old_pos.y, cam_position.z);
            cam_velocity = vec3(0.0f, 0.0f, cam_velocity.z);
        }
        if(cam_position.x < config::CAM_MIN_X){
            cam_position.x = config::CAM_MIN_X;
            cam_velocity.x = 0.0f;
        }
        if(cam_position.x > config::CAM_MAX_X){
            cam_position.x = config::CAM_MAX_X;
            cam_velocity.x = 0.0f;
        }
        if(cam_position.y < config::CAM_MIN_Y){
            cam_position.y = config::CAM_MIN_Y;
            cam_velocity.y = 0.0f;
        }
        if(cam_position.y > config::CAM_MAX_Y){
            cam_position.y = config::CAM_MAX_Y;
            cam_velocity.y = 0.0f;
        }
        if(cam_position.z < config::CAM_MIN_Z){
            cam_position.z = config::CAM_MIN_Z;
            cam_velocity.z = 0.0f;
        }
        if(cam_position.z > config::CAM_MAX_Z){
            cam_position.z = config::CAM_MAX_Z;
            cam_velocity.z = 0.0f;
        }
        camera::setPosition(cam_position);
    }


    static void drawEntities(){
        default_shader.load();

        mat4 view_projection = camera::getProjectionMatrix() * camera::getViewMatrix();
        std::vector<float> instance_data;

        for(ecs::CID cid = 0; cid < ecs::particle_data.vector.size(); cid++){
             
            ecs::PhysicsBody &body = ecs::physics_bodies.vector[ecs::physics_bodies.cid_map[ecs::particle_data.id_map[cid]]];

            vec2 pos = body.position;
            //float angle = body.theta;
            float radius = body.radius;
            vec3 color = ecs::particle_data.vector[cid].color;

            instance_data.push_back(pos.x);
            instance_data.push_back(pos.y);
            instance_data.push_back(radius);
            instance_data.push_back(color[0]);
            instance_data.push_back(color[1]);
            instance_data.push_back(color[2]);
        }

        for(ecs::CID cid = 0; cid < ecs::creature_data.vector.size(); cid++){
            if(!(ecs::creature_data.vector[cid].state & ecs::CreatureData::ALIVE)){
                continue;
            }
            ecs::PhysicsBody &body = ecs::physics_bodies.vector[ecs::physics_bodies.cid_map[ecs::creature_data.id_map[cid]]];

            vec2 pos = body.position;
            float angle = body.theta;
            float radius = body.radius;
            //vec3 color = body.color;

            mat4 model_matrix = mat4(1.0f);
            model_matrix = glm::translate(model_matrix, vec3(pos.x, pos.y, 0.0f));
            model_matrix = glm::rotate(model_matrix, -angle, vec3(0.0f, 0.0f, -1.0f));
            model_matrix = glm::scale(model_matrix, vec3(radius, radius, radius));
            mat4 MVP = view_projection * model_matrix;
            default_shader.setUniformMat4(0, MVP);
            Mesh &mesh = ecs::meshes[ecs::creature_data.vector[cid].mesh_id];
            if(mesh.vertex_buffer.size() > 0){
                mesh.upload(Mesh::STATIC, false);
            }
            mesh.draw();
        }

        if(instance_data.size() > 0){
            default_instance_shader.load();
            default_instance_shader.setUniformMat4(0, view_projection);
            circle.drawInstances(instance_data, Mesh::STREAM);
        }


    }

    static void drawGui(ecs::CID UI_source){
        assert(UI_state >= 0 && UI_state <= 4);
        string section_names[4] = {"Stats", "Genetics", "Physics", "Environment"};
        
        
        std::vector<string> stats;

        if(UI_state == 1 && UI_source != ecs::INVALID_CID){

            ecs::CreatureData &creature = ecs::creature_data.vector[UI_source];
            stats = {
                "name",
                creature.name,
                "generation",
                to_string(creature.generations),
                "mutations",
                to_string(creature.mutations),
                "energy",
                f_to_str(creature.energy),
                "neurons firing",
                to_string(creature.number_neurons_firing),
                "state",
                to_string(creature.state),
                "body sides",
                to_string(creature.appendage_count),
                "size",
                f_to_str(creature.size),
                "metabolic rate",
                f_to_str(creature.metabolic_rate),
                "input rate",
                f_to_str(creature.brain_input_rate),
                "leak rate",
                f_to_str(creature.brain_leak_rate),
                "carnivore",
                f_to_str(creature.carnivore),
                "sex",
                f_to_str(creature.sex),
                "mutation rate",
                f_to_str(creature.mutation_rate),
            };
        }

        if(UI_state == 2 && UI_source != ecs::INVALID_CID){
            ecs::CID cid = UI_source;

            
            ecs::CreatureData &creature = ecs::creature_data.vector[cid];
            string DNA_string = hex_to_str(creature.dna, config::CREATURE_DNA_SIZE);


            stats = {};
            for(int i = 0; i < config::CREATURE_DNA_SIZE / 8; i++){
                stats.push_back(DNA_string.substr(i * 16, 8));
                stats.push_back(DNA_string.substr(8 + i * 16, 8));
            }
        }

        if(UI_state == 3 && UI_source != ecs::INVALID_CID){
            ecs::CID cid = UI_source;
            ecs::ID id = ecs::creature_data.id_map[cid];
			if (id != ecs::INVALID_ID) {
				cid = ecs::physics_bodies.cid_map[id];
				if (cid != ecs::INVALID_CID) {
					ecs::PhysicsBody& body = ecs::physics_bodies.vector[cid];
					stats = {
						"radius",
						f_to_str(body.radius),
						"mass",
						f_to_str(body.mass)
					};
				}
			}
            
        }

        if(UI_state == 4){
            stats = {
                "n creatures",
                to_string(ecs::cellsAlive),
                "n entities",
                to_string(ecs::entitiesAlive),
                "n plant target",
                to_string((int)environment::getGrowthRate()),
                "fast forward",
                UI_sim_state_info
            };
        }

        if(UI_state > 0){
            float boxHeight = 0.05 + stats.size() / 2 * 0.023;
            float box_x = 0.6f;
            float box_width = 0.38f;
            gui::drawBox(box_x + 0.0f, 1.0f - 0.02f - boxHeight, box_width, boxHeight, COLOR_DARKGRAY);
            gui::drawStringUnscaled(section_names[UI_state-1], box_x + 0.05f, 0.96, 0.04, COLOR_WHITE);

            for(size_t i = 0; i < stats.size() / 2; i++){
                gui::drawStringUnscaled(stats[i * 2 + 0], box_x + 0.02f, 0.92 - i * 0.023, 0.03, COLOR_WHITE);
                gui::drawStringUnscaled(stats[i * 2 + 1], box_x + 0.17f, 0.92 - i * 0.023, 0.03, COLOR_WHITE);
            }
        }
        gui::render();
    }

    void moveCamera(vec3 direction){
        cam_input_vector = direction;
    }

    void setRenderMode(int mode){
        UI_state = mode;
    }

    void setStateInfo(string info){
        UI_sim_state_info = info;
    }

    vec2 cursorToWorld(int mx, int my){
        return camera::screenToWorld(mx, my);
    }
}