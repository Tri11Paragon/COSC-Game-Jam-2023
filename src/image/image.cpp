//
// Created by administrator on 22/02/23.
//
#define STB_IMAGE_IMPLEMENTATION
#include <image/image.h>

#include <blt/std/logging.h>

std::unique_ptr<image::Texture> image::loadImage(const std::string& path) {
    int width,height,channels;
    auto pixels = stbi_load(path.c_str(), &width, &height, &channels, 0);
    
    int pitch = (width * channels + 3) & ~3;
    
    unsigned int Rmask, Gmask, Bmask, Amask;
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
    Rmask = 0x000000FF;
    Gmask = 0x0000FF00;
    Bmask = 0x00FF0000;
    Amask = (channels == 4) ? 0xFF000000 : 0;
#else
    int s = (channels == 4) ? 0 : 8;
    Rmask = 0xFF000000 >> s;
    Gmask = 0x00FF0000 >> s;
    Bmask = 0x0000FF00 >> s;
    Amask = 0x000000FF >> s;
#endif
    
    SDL_Surface* surface = SDL_CreateRGBSurfaceFrom(pixels, width, height, channels*8, pitch, Rmask, Gmask, Bmask, Amask);
    
    if (!surface) {
        BLT_WARN("Unable to load texture!\n\tData will be deleted and null returned!");
        stbi_image_free(pixels);
        return nullptr;
    }
    
    return std::make_unique<image::Texture>(surface, pixels);
}
