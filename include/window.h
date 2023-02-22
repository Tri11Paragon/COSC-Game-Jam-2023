//
// Created by administrator on 22/02/23.
//

#ifndef COSC_GAME_JAM_2023_WINDOW_H
#define COSC_GAME_JAM_2023_WINDOW_H

#include <SDL2/SDL.h>

class IKeyEventListener;
class IMouseEventListener;

struct Window {
    int WIDTH = 1440;
    int HEIGHT = 720;
    bool running = true;
    SDL_Renderer* renderer = nullptr;
    SDL_Window* window = nullptr;
    
    void prepare() const;
    void handleInput ();
    void sync(double target) const;
    
    static double delta();
    static double deltaSeconds();
    
    static bool keyDown(SDL_Keycode key);
    static bool keyState(SDL_Keycode key);
    static bool mouseDown(int button);
    static bool mouseState(int button);
    
    static void registerListener(IKeyEventListener* listener);
    static void registerListener(IMouseEventListener* listener);
    static void deleteListener(IKeyEventListener* listener);
    static void deleteListener(IMouseEventListener* listener);
};

class IKeyEventListener {
    public:
        IKeyEventListener(){
            Window::registerListener(this);
        }
        ~IKeyEventListener(){
            Window::deleteListener(this);
        }
        virtual void keyPressed(SDL_KeyCode key) = 0;
        virtual void keyReleased(SDL_KeyCode key) = 0;
};

class IMouseEventListener {
    public:
        IMouseEventListener() {
            Window::registerListener(this);
        }
        ~IMouseEventListener() {
            Window::deleteListener(this);
        }
        virtual void mousePressed(int button, int x, int y) = 0;
        virtual void mouseReleased(int button, int x, int y) = 0;
};

extern Window window;

#endif //COSC_GAME_JAM_2023_WINDOW_H
