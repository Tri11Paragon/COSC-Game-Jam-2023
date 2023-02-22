#include <iostream>
#include <blt/std/logging.h>

#include <cstdio>
#include <SDL2/SDL.h>

#include <window.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

Window window {};

void handleInput (){
    SDL_Event event;
    
    while (SDL_PollEvent(&event)){
        switch (event.type) {
            case SDL_QUIT:
                window.running = false;
                BLT_INFO("Goodbye!");
                std::exit(0);
                break;
            default:
                break;
        }
    }
}

void prepareScreen(){
    SDL_SetRenderDrawColor(window.renderer, 96, 128, 255, 255);
    SDL_RenderClear(window.renderer);
    
    
}

void mainLoop(){
    prepareScreen();
    handleInput();

    SDL_Rect square = { 0, 0, 100, 100 };
    SDL_SetRenderDrawColor(window.renderer, 0xFF, 0x00, 0x00, 0xFF);
    SDL_RenderFillRect(window.renderer, &square);

    SDL_RenderPresent(window.renderer);
    
    // TODO: timer
    SDL_Delay(16);
}

int main() {
    auto logging_properties = blt::logging::LOG_PROPERTIES{true, true, true, "./"};
    logging_properties.m_logFullPath = true;

#ifdef __EMSCRIPTEN__
    logging_properties = blt::logging::LOG_PROPERTIES{false, true, false, "./"};
#endif
    
    blt::logging::init(logging_properties);
    
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
        BLT_FATAL("Unable to init SDL! (%s)", SDL_GetError());
        return 0;
    }
    
    SDL_version version;
    SDL_GetVersion(&version);
    
    BLT_INFO("Using SDL version %d.%d.%d", version.major, version.minor, version.patch);
    
    window.WIDTH = 1440;
    window.HEIGHT = 720;
    
    window.window = SDL_CreateWindow("Femboy Domination Fantasy XXL", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, window.WIDTH, window.HEIGHT, 0);
    
    if (!window.window){
        BLT_FATAL("Unable to create window! (%s)", SDL_GetError());
        return 2;
    }
    
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
    
    window.renderer = SDL_CreateRenderer(window.window, -1, SDL_RENDERER_ACCELERATED);
    
    if (!window.renderer) {
        BLT_FATAL("Unable to create renderer! (%s)", SDL_GetError());
        return 5;
    }

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(mainLoop, 0, true);
#else
    while(window.running)
        mainLoop();
#endif
    
    SDL_Quit();
    
    return 0;
}
