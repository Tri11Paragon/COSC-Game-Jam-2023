#include <iostream>
#include <blt/std/logging.h>

#include <window.h>
#include "image/image.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

Window window{};

class Background {
    public:
        SDL_Rect src = {0, 0, 540, 180};
        SDL_Rect dest{};
        std::unique_ptr<image::Texture> bb, back, mid, front;
        
        void setMode(bool dark) { // false = light, true = dark
            std::unique_ptr<image::Texture> t;
            if (dark) {
                bb = image::loadImage("assets/dark_bg_backback.png");
                back = image::loadImage("assets/dark_bg_back.png");
                mid = image::loadImage("assets/dark_bg_mid.png");
                front = image::loadImage("assets/dark_bg_front.png");
            } else {
                bb = image::loadImage("assets/light_bg_backback.png");
                back = image::loadImage("assets/light_bg_back.png");
                mid = image::loadImage("assets/light_bg_mid.png");
                front = image::loadImage("assets/light_bg_front.png");
            }
        }
        
        void draw() {
            dest = {window.scroll, 0, 2160, 720};
            SDL_RenderCopy(window.renderer, bb->texture, &src, &dest);
            SDL_RenderCopy(window.renderer, back->texture, &src, &dest);
            SDL_RenderCopy(window.renderer, mid->texture, &src, &dest);
            SDL_RenderCopy(window.renderer, front->texture, &src, &dest);
        }
};

class Object {
    public:
        float x, y;
        float w, h;
        std::unique_ptr<image::Texture> texture = nullptr;
        
        Object(float x, float y, float w, float h): x(x), y(y), w(w), h(h) {}
        
        void setTexture(const std::string& filePath) {
            texture = image::loadImage(filePath);
        }
        
        void draw() const {
            SDL_Rect dest = {(int) x, (int) y, (int) w, (int) h};
            if (texture == nullptr) {
                SDL_SetRenderDrawColor(window.renderer, 0xAA, 0xAA, 0xAA, 0xFF);
                SDL_RenderFillRect(window.renderer, &dest);
            } else {
                SDL_Rect src = {0, 0, (int) w, (int) h};
                SDL_RenderCopy(window.renderer, texture->texture, &src, &dest);
            }
        }
        
        [[nodiscard]] bool intersects(const Object& o) const {
            bool hcond = x + (float) w > o.x && o.x + (float) o.w > x;
            bool vcond = y + (float) h > o.y && o.y + (float) o.h > y;
            return hcond && vcond;
        }
};

class Platform : public Object {
    public:
        Platform(int x, int y, int w): Object(x, y, w, 10) {}
};

class Player : public Object {
    public:
        float dx = 0.0f, dy = 0.0f;
        bool jump = false;
        
        Player(float x, float y): Object(x, y, 75, 75) {}
        
        void move() {
            const float jump_speed = 55;
            const float move_speed = 25;
            const float gravity = 9.8;
            
            if (Window::keyDown(SDLK_a))
                dx -= 0.2f * move_speed;
            else if (Window::keyDown(SDLK_d))
                dx += 0.2f * move_speed;
            if (Window::keyDown(SDLK_SPACE) && !jump) {
                dy = -8.0f * jump_speed;
                jump = true;
            }
            
            dy += gravity; // Gravity
            
            x += (float)(dx * Window::deltaSeconds());
            y += (float)(dy * Window::deltaSeconds());
        }
        
        bool intersects(const Object& o) {
            bool cond = Object::intersects(o);
            if (cond)
                jump = false;
            return cond;
        }
};

float count = 0.0f;
Player guy{100, 100};
Platform a{100, 400, 300};
Background bg;

void init() {
    guy.setTexture("assets/dark_player.png");
    bg.setMode(true);
}

void mainLoop() {
    window.prepare();
    window.handleInput();
    
    guy.move();
    if (guy.intersects(a)) {
        while (guy.intersects(a))
            guy.y -= (float)(1.0 * Window::deltaSeconds());
        guy.dy = 0;
    }
    bg.draw();
    guy.draw();
    a.draw();
    
    
    window.sync(1000.0 / 60.0);

//    BLT_TRACE("Delta %f", Window::delta());
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
    
    if (!window.window) {
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
    while (window.running)
        mainLoop();
#endif
    
    SDL_Quit();
    
    return 0;
}
