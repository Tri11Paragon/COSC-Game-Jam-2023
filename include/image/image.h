//
// Created by administrator on 22/02/23.
//

#ifndef COSC_GAME_JAM_2023_IMAGE_H
#define COSC_GAME_JAM_2023_IMAGE_H

#include <image/stb_image.h>
#include <SDL2/SDL.h>
#include <string>
#include <memory>
#include <window.h>

namespace image {
    
    struct Texture {
        SDL_Surface* surface;
        SDL_Texture* texture;
        
        Texture(SDL_Surface* surface, unsigned char* data): data(data), surface(surface) {
            texture = SDL_CreateTextureFromSurface(window.renderer, surface);
        }
        
        ~Texture() {
            SDL_DestroyTexture(texture);
            SDL_FreeSurface(surface);
            stbi_image_free(data);
        }
        
        private:
            unsigned char* data;
    };
    
    std::unique_ptr<Texture> loadImage(const std::string& path);
    
}

#endif //COSC_GAME_JAM_2023_IMAGE_H
