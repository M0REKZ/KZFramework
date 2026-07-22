#ifndef PLUSKAIZO_IMAGES_H
#define PLUSKAIZO_IMAGES_H

#include <SDL3/SDL_surface.h>
#include <unordered_map>

class CKZImages
{
    std::unordered_map<unsigned int, SDL_Surface *> LoadedImages;

    public:

    unsigned int LoadImage(unsigned int img);
    void RenderImage(unsigned int img, int x_from, int y_from, int w_from, int h_from, int x_to, int y_to, int w_to, int h_to);
    void UnloadAllImages();
    void UnloadImage(unsigned int img);
};

extern CKZImages KZImages;

#endif
