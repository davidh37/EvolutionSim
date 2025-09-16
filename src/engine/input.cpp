#include "input.hpp"
#include "common.hpp"
#include <SDL2/SDL.h>

namespace input {
    static bool is_quitting = false;
    static bool window_size_changed = false;
    static int8_t keystates[KEY_TOTAL];

    static uint32_t keycode_to_SDL[KEY_BOARD_TOTAL] = {
        0,
        SDLK_a, SDLK_b, SDLK_c, SDLK_d, SDLK_e, SDLK_f, SDLK_g, SDLK_h, SDLK_i, SDLK_j, SDLK_k, SDLK_l, SDLK_m,
        SDLK_n, SDLK_o, SDLK_p, SDLK_q, SDLK_r, SDLK_s, SDLK_t, SDLK_u, SDLK_v, SDLK_w, SDLK_x, SDLK_y, SDLK_z,
        SDLK_UP, SDLK_LEFT, SDLK_DOWN, SDLK_RIGHT,
        SDLK_DELETE, SDLK_SPACE, SDLK_LSHIFT, SDLK_LCTRL, SDLK_ESCAPE,
        SDLK_0, SDLK_1, SDLK_2, SDLK_3, SDLK_4, SDLK_5, SDLK_6, SDLK_7, SDLK_8, SDLK_9,
        SDLK_F1, SDLK_F2, SDLK_F3, SDLK_F4, SDLK_F5, SDLK_F6,
        SDLK_F7, SDLK_F8, SDLK_F9, SDLK_F10, SDLK_F11, SDLK_F12
    };

    void queryInputs(){

        //cout << "QUERY" << endl;
        for(int i = 0; i < KEY_TOTAL; i++){
            if(keystates[i] == RELEASED){
                keystates[i] = UP;
            }
            if(keystates[i] == PRESSED){
                keystates[i] = DOWN;
            }
        }

        //manually create up event for mouse wheel
        if(keystates[KEY_MOUSE_WHEEL_DOWN] == DOWN){
            keystates[KEY_MOUSE_WHEEL_DOWN] = RELEASED;
        }
        if(keystates[KEY_MOUSE_WHEEL_UP] == DOWN){
            keystates[KEY_MOUSE_WHEEL_UP] = RELEASED;
        }

        window_size_changed = false;

        SDL_Event event;
        while(SDL_PollEvent(&event)){
            if (event.type == SDL_QUIT) {
                is_quitting = true;
            }else if(event.type == SDL_WINDOWEVENT){
                if(event.window.event == SDL_WINDOWEVENT_CLOSE){
                    is_quitting = false;
                }if(event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED){
                    window_size_changed = true;
                }
            }else if(event.type == SDL_KEYDOWN){
                uint32_t key = event.key.keysym.sym;
                for(int k = 1; k < KEY_BOARD_TOTAL; k++){
                    if(key == keycode_to_SDL[k]){
                        if(keystates[k] <= 0){
                            keystates[k] = PRESSED;
                        }
                    }
                }
            }else if(event.type == SDL_KEYUP){
                uint32_t key = event.key.keysym.sym;
                for(int k = 1; k < KEY_BOARD_TOTAL; k++){
                    if(key == keycode_to_SDL[k]){
                        keystates[k] = RELEASED;
                    }
                }
            }else if(event.type == SDL_MOUSEWHEEL){
                if(event.wheel.y <= -1){
                    if(!(keystates[KEY_MOUSE_WHEEL_DOWN] & DOWN)){
                        keystates[KEY_MOUSE_WHEEL_DOWN] = PRESSED;
                    }
                }else if(event.wheel.y >= 1){
                    if(!(keystates[KEY_MOUSE_WHEEL_UP] & DOWN)){
                        keystates[KEY_MOUSE_WHEEL_UP] = PRESSED;
                    }
                }
            }else if(event.type == SDL_MOUSEBUTTONDOWN){
                if(event.button.button == SDL_BUTTON_LEFT){
                    if(keystates[KEY_MOUSE_LEFT] <= 0){
                        keystates[KEY_MOUSE_LEFT] = PRESSED;
                    }
                }else if(event.button.button == SDL_BUTTON_RIGHT){
                    if(keystates[KEY_MOUSE_RIGHT] <= 0){
                        keystates[KEY_MOUSE_RIGHT] = PRESSED;
                    }
                }
            }else if(event.type == SDL_MOUSEBUTTONUP){
                if(event.button.button == SDL_BUTTON_LEFT){
                    keystates[KEY_MOUSE_LEFT] = RELEASED;
                }else if(event.button.button == SDL_BUTTON_RIGHT){
                    keystates[KEY_MOUSE_RIGHT] = RELEASED;
                }
            }
        }
    }

    bool hasQuit(){
        return is_quitting;
    }

    bool hasResized(){
        return window_size_changed;
    }

    void getMousePosition(int &x, int &y){
        SDL_GetMouseState(&x, &y);
    }
    Keystate getKeyState(Keycode key){
        return (Keystate)keystates[key];
    }
}
