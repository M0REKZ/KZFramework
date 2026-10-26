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

#ifndef PLUSKAIZO_TEXT_H
#define PLUSKAIZO_TEXT_H

#include <unordered_map>
#include <SDL3_ttf/SDL_ttf.h>

class CKZText
{
    //for now im just using C++ char
    std::unordered_map<Uint32, SDL_Surface *> TextCharCache;
    std::unordered_map<Uint32, SDL_Surface *> TextCharOutlineCache;
    TTF_Font * LoadedText = nullptr;
    SDL_Color CurrentColor = {0,0,0,0};
    bool Outline = false;
public:
    bool Init();
    void Destroy();

    bool LoadText(const char * pfile);
    bool InitText();
    void SetOutline(bool state);
    void ClearCache();
    bool AddCharacter(const unsigned char * p_ch);
    bool AddCharSet(std::string epiccharset);
    void SetCurrentColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a);
    void RenderText(std::string epictexttorender, int x, int y);
    void CalculateRenderSize(std::string epictexttorender, int * out_x, int * out_y);
};

extern CKZText KZText;

#endif 
