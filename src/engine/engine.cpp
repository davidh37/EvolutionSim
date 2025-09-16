#include "glad/glad/glad.h"
#include <SDL2/SDL.h>
#include "common.hpp"
#include "engine.hpp"


namespace engine {

    static SDL_Window *window = nullptr;
    static SDL_GLContext context = nullptr;
    
    

    void initialize(){
        
        if(SDL_Init(SDL_INIT_VIDEO) < 0){
            cout << "SDL could not initialize\n";
            abort();
        }

    }

    void createGLWindow(string window_name, int res_x, int res_y, bool resizable, int gl_major, int gl_minor){

        Uint32 flag = resizable ? (SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE) : SDL_WINDOW_OPENGL;
        window = SDL_CreateWindow(window_name.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, res_x, res_y, flag);
        if (window == NULL) {
            cout << "SDL could not create window\n";
            SDL_Quit();
            abort();
        }
        
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, gl_major);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, gl_minor);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        
        context = SDL_GL_CreateContext(window);
        if(context == NULL){
            cout << "Failed to create GL context\n";
            SDL_DestroyWindow(window);
            SDL_Quit();
            abort();
        }
        SDL_GL_SetSwapInterval(0); //vsync off

        if(!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)){
            cout << "GLAD could not initalize\n";
            SDL_GL_DeleteContext(context);
            SDL_DestroyWindow(window);
            SDL_Quit();
            abort();
        }

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        //glEnable(GL_DEPTH); // required for Z
    }

    

    uint64 getMs(){
        return SDL_GetTicks64();
    }

	void sleep(uint32 t){
        SDL_Delay(t);
    }

    void getWindowSize(int &w, int &h){
        SDL_GetWindowSize(window, &w, &h);
    }

    void getScreenSize(int &w, int &h){
        SDL_DisplayMode display_mode;
        SDL_GetCurrentDisplayMode(0, &display_mode);
        w = display_mode.w;
        h = display_mode.h;
    }
    
	void setViewport(int x, int y, int w, int h){
        glViewport(x, y, w, h);
    }

	void swapBuffer(){
        SDL_GL_SwapWindow(window);
    }

    void clearScreen(float r, float g, float b){
        glClearColor(r, g, b, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    void quit(){
        SDL_GL_DeleteContext(context);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }
}