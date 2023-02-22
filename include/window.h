//
// Created by administrator on 22/02/23.
//

#ifndef COSC_GAME_JAM_2023_WINDOW_H
#define COSC_GAME_JAM_2023_WINDOW_H

struct Window {
    int WIDTH = 1440;
    int HEIGHT = 720;
    bool running = true;
    SDL_Renderer* renderer = nullptr;
    SDL_Window* window = nullptr;
};

extern Window window;

#endif //COSC_GAME_JAM_2023_WINDOW_H
