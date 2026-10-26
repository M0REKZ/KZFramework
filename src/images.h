/*
    KZFramework
    Copyright (C) Benjamín Gajardo

    This file is part of KZFramework.

    KZFramework is free software: you can redistribute it and/or modify it under the terms of
    the GNU Lesser General Public License as published by the Free Software Foundation, either
    version 3 of the License, or (at your option) any later version.

    KZFramework is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
    without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
    See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with KZFramework.
    If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef PLUSKAIZO_IMAGES_H
#define PLUSKAIZO_IMAGES_H

#include <SDL3/SDL_surface.h>
#include <unordered_map>

class CKZImages
{
    std::unordered_map<unsigned int, SDL_Surface *> LoadedImages;

    public:

    unsigned int LoadImage(unsigned int img);
    bool IsImageSlotBusy(unsigned int img);
    void RenderImage(unsigned int img, int x_from, int y_from, int w_from, int h_from, int x_to, int y_to, int w_to, int h_to);
    void RenderRotoZoom(unsigned int img, int x_from, int y_from, int w_from, int h_from, int cx_to, int cy_to, double zoomx, double zoomy, double angle);
    void KeepAreaAsImage(int x_from, int y_from, int w_from, int h_from, unsigned int destimg);
    void UnloadAllImages();
    void UnloadImage(unsigned int img);

    unsigned int CopyImageFor3D(unsigned int img);
    void Set3DImage(unsigned int img3d);

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
