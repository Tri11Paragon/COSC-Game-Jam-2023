//
// Created by administrator on 22/02/23.
//
#include <window.h>
#include "blt/std/logging.h"

void Window::handleInput() {
    SDL_Event event;
    
    while (SDL_PollEvent(&event)){
        switch (event.type) {
            case SDL_QUIT:
                running = false;
                BLT_INFO("Goodbye!");
                std::exit(0);
                break;
            case SDL_KEYDOWN:
                break;
            case SDL_KEYUP:
                break;
            case SDL_MOUSEBUTTONDOWN:
                break;
            case SDL_MOUSEBUTTONUP:
                break;
            default:
                break;
        }
    }
}