//
// Created by administrator on 22/02/23.
//

#ifndef COSC_GAME_JAM_2023_IMAGE_H
#define COSC_GAME_JAM_2023_IMAGE_H

#include <image/stb_image.h>
#include <SDL2/SDL.h>
#include <string>
#include <memory>

namespace image {
    
    struct Texture {
        SDL_Surface* surface;
        
        Texture(SDL_Surface* surface, unsigned char* data): data(data), surface(surface) {}
        
        ~Texture() {
            SDL_FreeSurface(surface);
            stbi_image_free(data);
        }
        
        inline SDL_Surface* operator()() const {
            return surface;
        }
        
        inline SDL_Surface* operator*() const {
            return surface;
        }
        
        inline SDL_Surface& operator->() const {
            return *surface;
        }
        
        private:
            unsigned char* data;
    };
    
    std::unique_ptr<Texture> loadImage(const std::string& path);
    
}

#endif //COSC_GAME_JAM_2023_IMAGE_H
