#include <iostream>
#include <blt/std/logging.h>

#include <window.h>
#include "image/image.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

Window window {};

class Object {
public:
    float x, y;
    int w, h;
    SDL_Texture* texture;
    Object(float x, float y, int w, int h): x(x), y(y), w(w), h(h) { }
    void setTexture(char* filePath) {
        std::unique_ptr<image::Texture> t = image::loadImage(filePath);
        texture = SDL_CreateTextureFromSurface(window.renderer, t->surface);

//        Uint32 rf = SDL_PIXELFORMAT_RGBA8888;
//        Uint32 f;
//        int *w, *h;
//        SDL_QueryTexture(texture, &f, NULL, w, h);
//        std::cerr << "Format: " << SDL_GetPixelFormatName(f);
    }
    void draw() {
        SDL_Rect dest = {(int) x, (int) y, w, h};
        if (texture == nullptr) {
            SDL_SetRenderDrawColor(window.renderer, 0xAA, 0xAA, 0xAA, 0xFF);
            SDL_RenderFillRect(window.renderer, &dest);
        }
        else {
            SDL_Rect src = {0, 0, w, h};
            SDL_RenderCopy(window.renderer, texture, &src, &dest);
        }
    }
    bool intersects(Object o) {
        bool hcond = x + w > o.x || o.x + o.w > x;
        bool vcond = y + h > o.h || o.y + o.h > y;
        return hcond && vcond;
    }
};

class Platform: public Object {
public:
    Platform(int x, int y, int w): Object(x, y, w, 10) { }
};

class Player: public Object{
public:
    float dx = 0.0f, dy = 0.0f;
    Player(int x, int y): Object(x, y, 75, 75) { }
    void move() {
        if (window.keyDown(SDLK_a))
            dx -= 0.2f;
        else if (window.keyDown(SDLK_d))
            dx += 0.2f;
        if (window.keyDown(SDLK_w))
            dy -= 0.2f;
        else if (window.keyDown(SDLK_s))
            dy += 0.2f;

        x += dx;
        y += dy;
    }
};

float count = 0.0f;
Player guy {100, 100};
Platform a {100, 400, 300};

void init(){
    guy.setTexture("assets/winuz.png");
}

void mainLoop(){
    window.prepare();
    window.handleInput();

//    guy.x = 50 * sin((float) count) + 100;
//    guy.y = 50 * cos((float) count) + 100;
//    count += 0.1f;
    guy.move();
    guy.draw();
    a.draw();
    
    window.sync(1000.0 / 60.0);
    
    BLT_TRACE("Delta %f", Window::delta());
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

    init();

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(mainLoop, 0, true);
#else
    while(window.running)
        mainLoop();
#endif
    
    SDL_Quit();
    
    return 0;
}
