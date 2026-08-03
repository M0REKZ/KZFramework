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

    inline int GetImageWidth(unsigned int img)
    {
        try
        {
            return LoadedImages.at(img)->w;
        }
        catch(const std::out_of_range & e)
        {
            printf("Could not get image %u: %s\n", img, e.what());
            return 0;
        }
    }

    inline int GetImageHeight(unsigned int img)
    {
        try
        {
            return LoadedImages.at(img)->h;
        }
        catch(const std::out_of_range & e)
        {
            printf("Could not get image %u: %s\n", img, e.what());
            return 0;
        }
    }
};

extern CKZImages KZImages;

#endif
