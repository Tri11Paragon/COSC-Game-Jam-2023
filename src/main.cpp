#include <iostream>
#include <blt/std/logging.h>
#include <blt/std/random.h>

#include <window.h>
#include <vector>
#include <algorithm>
#include "image/image.h"
#include <blt/std/time.h>

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
            dest = {0, 0, 2160, 720};
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
        bool scrolls = true;
        std::unique_ptr<image::Texture> texture = nullptr;
        
        Object(float x, float y, float w, float h): x(x), y(y), w(w), h(h) {}
        
        void setTexture(const std::string& filePath) {
            texture = image::loadImage(filePath);
        }
        
        void draw() {
            SDL_Rect dest = {(int) (x), (int) y, (int) w, (int) h};
            if (texture == nullptr) {
                SDL_SetRenderDrawColor(window.renderer, 0xAA, 0xAA, 0xAA, 0xFF);
                SDL_RenderFillRect(window.renderer, &dest);
            } else {
                SDL_Rect src = {0, 0, (int) w, (int) h};
                SDL_RenderCopy(window.renderer, texture->texture, &src, &dest);
            }
            x -= (float)(scrolls ? Window::deltaSeconds() * 189 : 0);
        }
        
        [[nodiscard]] bool intersects(const Object& o) const {
            float ny = y + 5;
            bool hcond = x + (float) w > o.x && o.x + (float) o.w > x;
            bool vcond = ny + (float) h > o.y && o.y + (float) o.h > ny;
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
        
        Player(float x, float y): Object(x, y, 75, 75) {
            scrolls = false;
        }
        
        double time = 0;
        
        static double easeInOutCubic(double x) {
            return x < 0.5 ? 4 * x * x * x : 1 - pow(-2 * x + 2, 3) / 2;
        }
        
        static double blend(float min, float max, double modifier) {
            double difference = max - min;
            return min + difference * (modifier > 1 ? 1 : modifier);
        }
        
        void update() {
            const float jump_speed = 55;
            const float move_speed_min = 100;
            const float move_speed_max = 300;
            const float gravity = 9.8;
    
            auto blended = blend(move_speed_min, move_speed_max, easeInOutCubic(time));
            
            if (Window::keyDown(SDLK_a)) {
                x -= (float) (blended * Window::deltaSeconds());
                time += Window::deltaSeconds();
            }
            else if (Window::keyDown(SDLK_d)) {
                x += (float) (blended * Window::deltaSeconds());
                time += Window::deltaSeconds();
            } else
                time = 0;
            
            if (Window::keyDown(SDLK_SPACE) && !jump) {
                dy = -8.0f * jump_speed;
                jump = true;
            }
    
            dy += gravity; // Gravity
        }
        
        void move() {
            x += (float)(dx * Window::deltaSeconds());
            y += (float)(dy * Window::deltaSeconds());
        }
        
        [[nodiscard]] bool intersects(const Object& o) {
            auto cond = Object::intersects(o);
            if (cond)
                jump = false;
            return cond;
        }
};

float count = 0.0f;
double aliveTime = 0;
Player guy{100, 100};

std::vector<Platform> platforms{};
Background bg;

void init() {
    guy.setTexture("assets/dark_player.png");
    bg.setMode(true);
    platforms.emplace_back(100, 400, 300);
}

blt::random<float> sizeGenerator(65, 300, blt::system::getCurrentTimeNanoseconds());
blt::random<float> widthGenerator(55, 145, blt::system::getCurrentTimeNanoseconds());
blt::random<float> heightGenerator(-200, 200, blt::system::getCurrentTimeNanoseconds());


void mainLoop() {
    if (!window.running)
        return;
    window.prepare();
    window.handleInput();
    
    guy.update();
    bg.draw();
    for (auto& platform : platforms) {
        if (guy.intersects(platform)) {
            if (guy.dy > 0)
                guy.dy = 0;
        }
        platform.draw();
    }
    guy.move();
    
    platforms.erase(std::remove_if(platforms.begin(), platforms.end(), [](const Platform& p) -> bool {
        return p.x + p.w + 10 < 0;
    }), platforms.end());
    
    if (platforms.size() < 5){
        int width, height;
        SDL_GetWindowSize(window.window, &width, &height);
        
        const auto& p = platforms[platforms.size()-1];
        float newX = p.x + p.w + widthGenerator.get();
        float newY = (float)height / 2.0f + heightGenerator.get();
        if (newY < 0)
            newY = 0;
        if (newY > (float)height)
            newY = (float)height - 55;
        
        platforms.emplace_back((int)newX, (int)newY, (int)sizeGenerator.get());
    }
    guy.draw();
    
    if (guy.y < -guy.h) {
        // guy has fallen
        window.running = false;
    } else {
        aliveTime += Window::deltaSeconds() * 100;
    }
    
    
    window.scroll -= (float) Window::deltaSeconds() * 25;
    window.sync(1000.0 / 60.0);
    
    std::string title = "FPS ";
    title += std::to_string(1000 / Window::delta());
    title += " | Score: ";
    title += std::to_string((int)aliveTime);
    SDL_SetWindowTitle(window.window, title.c_str());
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
