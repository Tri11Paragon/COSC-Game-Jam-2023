#include <iostream>
#include <blt/std/logging.h>

#include <cstdio>
#include <SDL2/SDL.h>

#include <window.h>

#define STB_IMAGE_IMPLEMENTATION
#include <image/stb_image.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

Window window {};

class Player {
public:
    int x;
    int y;
    Player(int x, int y): x(x), y(y) {
//        this->x = x;
//        this->y = y;
    }
private:
    unsigned int imagedata[4] = {0xff0000ffu, 0x00ff00ffu, 0x0000ffffu, 0xffffffffu};
    SDL_Rect src = {0, 0, 2, 2};
public:
    void draw() {
        SDL_Rect square = { this->x, this->y, 100, 100 };
        SDL_SetRenderDrawColor(window.renderer, 0xFF, 0xFF, 0xFF, 0xFF);
        SDL_RenderFillRect(window.renderer, &square);

        SDL_Texture* texture = SDL_CreateTexture( window.renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC, 2, 2 );
        SDL_UpdateTexture(texture, &src, &imagedata, 2);
        SDL_Rect dest = {this->x, this->y, 100, 100};
        SDL_RenderCopy(window.renderer, texture, &src, &dest);
    }
};

Player guy(100, 100);
float count = 0.0f;

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

    guy.x = 50 * sin((float) count) + 100;
    guy.y = 50 * cos((float) count) + 100;
    count += 0.1f;
    guy.draw();

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
    
    SDL_CreateTexture()
    
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
