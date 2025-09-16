#include "engine/common.hpp"
#include "engine/engine.hpp"
#include "engine/input.hpp"
#include "ecs.hpp"
#include "config.hpp"
#include "systems/creatures_generator.hpp"
#include "systems/creatures_physics_IO.hpp"
#include "systems/creatures_thinking.hpp"
#include "systems/creatures.hpp"
#include "systems/environment.hpp"
#include "systems/particles.hpp"
#include "systems/physics.hpp"
#include "systems/rendering.hpp"

static string TERMINAL_COLOR = "\033[1;36m";


void initialize(){
    engine::initialize();

    ecs::initialize();

    creatures_generator::initialize();
    creatures_physics_IO::initialize();
    creatures_thinking::initialize();
    creatures::initialize();
    environment::initialize();
    particles::initialize();
    physics::initialize();
    rendering::initialize();

    srand(config::SIM_SEED);

    cout << TERMINAL_COLOR + "[Main] initialize" + TERMINAL_CLEAR << std::endl;
}

void cleanup(){

    creatures_generator::cleanup();
    creatures_physics_IO::cleanup();
    creatures_thinking::cleanup();
    creatures::cleanup();
    environment::cleanup();
    particles::cleanup();
    physics::cleanup();
    rendering::cleanup();
    
    engine::quit();
    exit(0);

    cout << TERMINAL_COLOR + "[Main] cleanup" + TERMINAL_CLEAR << std::endl;
}

int processUI();

bool update(float real_delta){
    static uint64_t tick = 0;

    int state = processUI();

    if(!(state & 2)){
        // if not paused
        environment::update(tick);
        physics::update(tick);
        particles::update();
        creatures_generator::update();
        creatures_thinking::update();
        creatures_physics_IO::update();
        tick++;
    }
    rendering::update(real_delta);
    return state & 1;
}

int processUI(){
    input::queryInputs();

    // DETECT WINDOW RESIZING
    if(input::hasResized()){
		int x, y;
		engine::getWindowSize(x, y);
        rendering::resize(x, y);
    }

    // CAMERA INPUT
    vec3 camera_move_vector = vec3(0.0f, 0.0f, 0.0f);
    camera_move_vector[0] += (input::getKeyState(input::KEY_A) > 0) ? -1.0f : 0.0f;
    camera_move_vector[0] += (input::getKeyState(input::KEY_D) > 0) ? 1.0f : 0.0f;
    camera_move_vector[1] += (input::getKeyState(input::KEY_S) > 0) ? -1.0f : 0.0f;
    camera_move_vector[1] += (input::getKeyState(input::KEY_W) > 0) ? 1.0f : 0.0f;
    camera_move_vector[2] += (input::getKeyState(input::KEY_MOUSE_WHEEL_DOWN) > 0) ? -1.0f : 0.0f;
    camera_move_vector[2] += (input::getKeyState(input::KEY_MOUSE_WHEEL_UP) > 0) ? 1.0f : 0.0f;
    rendering::moveCamera(camera_move_vector);
    
    // ENVIRONMENT MANIPULATION
    int x, y;
    input::getMousePosition(x, y);
    vec2 mouseWorld = rendering::cursorToWorld(x, y);
    
    if(input::getKeyState(input::KEY_MOUSE_RIGHT) == input::DOWN){
        environment::spawnFood(mouseWorld);
    }
    if(input::getKeyState(input::KEY_UP) == input::DOWN || input::getKeyState(input::KEY_RIGHT) == input::PRESSED){
        environment::addGrowthRate(50.0f);
    }
    if(input::getKeyState(input::KEY_DOWN) == input::DOWN || input::getKeyState(input::KEY_LEFT) == input::PRESSED){
        environment::addGrowthRate(-10.0f);
    }

    // UI ENTITY SELECTION
    bool click = input::getKeyState(input::KEY_MOUSE_LEFT) == input::PRESSED;
    static bool continuous = false;
    if(input::getKeyState(input::KEY_C) == input::PRESSED){
        continuous = !continuous;
        creatures_generator::setBrainMeshing(continuous);
    }
    ecs::CID cid = physics::findBody(mouseWorld);
    static ecs::CID old_ui_cid = ecs::INVALID_CID;
    static ecs::CID old_highlighted_cid = ecs::INVALID_CID;


    // disable old ui source
    if(old_ui_cid != ecs::INVALID_CID && old_ui_cid < ecs::cellsAlive){
        ecs::creature_data.vector[old_ui_cid].ui_source = false;
        old_ui_cid = ecs::INVALID_CID;
    } 
    if(click){
        // disable old highlighted
        if(old_highlighted_cid != ecs::INVALID_CID && old_highlighted_cid < ecs::cellsAlive){
            ecs::creature_data.vector[old_highlighted_cid].highlighted = false;
            old_highlighted_cid = ecs::INVALID_CID;
        }
    }

    if(cid != ecs::INVALID_CID){
        ecs::ID id = ecs::physics_bodies.id_map[cid];
        assert(id != ecs::INVALID_ID);
        ecs::CID creature_cid = ecs::creature_data.cid_map[id];
        if(creature_cid != ecs::INVALID_CID){
            // enable new ui source if nothing highlighed
            ecs::creature_data.vector[creature_cid].ui_source = true;
            old_ui_cid = creature_cid;
            if(click){
                ecs::creature_data.vector[creature_cid].highlighted = true;
                ecs::creature_data.vector[creature_cid].to_mesh = true;
                old_highlighted_cid = creature_cid;
            }
        }
    }

    // CYCLE RENDER_MODE
    if(input::getKeyState(input::KEY_1) == input::PRESSED){
        rendering::setRenderMode(0);
    }
    if(input::getKeyState(input::KEY_2) == input::PRESSED){
        rendering::setRenderMode(1);
    }
    if(input::getKeyState(input::KEY_3) == input::PRESSED){
        rendering::setRenderMode(2);
    }
    if(input::getKeyState(input::KEY_4) == input::PRESSED){
        rendering::setRenderMode(3);
    }
    if(input::getKeyState(input::KEY_5) == input::PRESSED){
        rendering::setRenderMode(4);
    }

    // FAST_FORWARD?
    static bool ff = false;
    if(input::getKeyState(input::KEY_F) == input::PRESSED){
        ff = !ff;
    }

    static bool pause = false;
    if(input::getKeyState(input::KEY_SPACE) == input::PRESSED){
        pause = !pause;
    }

    // EXIT
    if(input::getKeyState(input::KEY_ESC) == input::PRESSED || input::hasQuit()){
        cleanup();
    }

    // SET STATE INFO
    if(pause){
        rendering::setStateInfo("paused");
    }else if(ff){
        rendering::setStateInfo("fast forward");
    }else{
        rendering::setStateInfo("normal");
    }

    return (int) ff + 2 * (int) pause;
}

int main(int argc, char *argv[]){

    initialize();

    double accumulator = 0.0;
    uint64 stampOld = engine::getMs();
    double dt = 0.0f;
    
    bool fast_forward = false;
    float delta = config::SIM_DELTA;
   
    while(true){
        uint64 stampNew = engine::getMs();
        dt = (double)(stampNew - stampOld) / 1000.0;
        stampOld = stampNew;

        if(fast_forward){ // fast forward
            fast_forward = update(dt);
        }else{
            accumulator += dt;
             if(accumulator >= delta){
                fast_forward = update(delta);
                accumulator -= delta;
            }else{
                engine::sleep(1);
            }
        }
    }
    cleanup();
}


