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

#include <string>
#include "window.h"
#include "text.h"
#include <iostream>
#include "files.h"

CKZText KZText;

bool CKZText::Init()
{
    if(!TTF_Init())
    {
        SDL_Log("ERROR INIT SDL_TTF: %s\n", SDL_GetError());
        return false;
    }

    SetCurrentColor(255, 255, 255, 255);

    return true;
}

void CKZText::Destroy()
{
    TTF_Quit();
}

bool CKZText::LoadText(const char *pfile)
{
    if(LoadedText)
    {
        TTF_CloseFont(LoadedText);
        LoadedText = nullptr;
    }

    std::string filename = KZFiles.GetBasePath() + "data/font/" + std::string(pfile) + ".ttf";

    TTF_Font * pFont = TTF_OpenFont(filename.c_str(), 20);

    if(pFont)
    {
        LoadedText = pFont;
        return true;
    }

    SDL_Log("ERROR: Could not load font %s!\n", filename.c_str());

    return false;
}

bool CKZText::InitText()
{
    bool res = true;

    //create basic cache
    unsigned char xd = '\0';
    for(xd = 0x21; xd < 0x7E; xd++)
    {
        if(!AddCharacter(&xd))
        {
            res = false;
        }
    }

    return res;
}

void CKZText::SetOutline(bool state)
{
    Outline = state;
}

void CKZText::ClearCache()
{
    for(auto surf : TextCharCache)
    {
        SDL_DestroySurface(surf.second);
    }
    TextCharCache.clear();

    for(auto surf : TextCharOutlineCache)
    {
        SDL_DestroySurface(surf.second);
    }
    TextCharOutlineCache.clear();
}

bool CKZText::AddCharacter(const unsigned char * p_ch)
{
    if(!LoadedText)
        return false;

    if(p_ch[0] == ' ')
        return false;

    Uint8 utf8size =
    !(*p_ch & 128) ? 1 :
    (
        ((*p_ch & (64 | 32)) == 64) ? 2 :
        (
            ((*p_ch & (64 | 32 | 16)) == (64 | 32)) ? 3 :
            (
                ((*p_ch & (64 | 32 | 16 | 8)) == (64 | 32 | 16)) ? 4 : 0
            )
        )
    );

    //first byte codepoint is not valid
    if(!utf8size)
    {
        SDL_Log("invalid utf8 character %d (next bytes %d %d %d)\n", p_ch[0], p_ch[1], p_ch[2], p_ch[3]);
        return false;
    }

    Uint32 ch = 0;
    ch = p_ch[0] << 24;
    if(utf8size > 1)
    {
        ch |= p_ch[1] << 16;
        if(utf8size > 2)
        {
            ch |= p_ch[2] << 8;
            if(utf8size > 3)
            {
                ch |= p_ch[3];
            }
        }
    }

    SDL_Log("adding %u %d %d %d %d (size %d)\n", ch, p_ch[0], p_ch[1], p_ch[2], p_ch[3] ,utf8size);

    Uint8 success = 0;
    
    SDL_Surface * pSur = TTF_RenderText_Blended(LoadedText, (char *)p_ch, utf8size, CurrentColor);

    if(pSur)
    {
        TextCharCache[ch] = pSur;
        
        SDL_Color color;
        color.r = 0;
        color.g = 0;
        color.b = 0;
        color.a = 255;
        SDL_Surface * pShadow = TTF_RenderText_Blended(LoadedText, (char *)p_ch, utf8size, color);
        if(pShadow)
        {
            SDL_Surface * pNewSur = SDL_CreateSurface(pSur->w + 2, pSur->h + 2, pSur->format);
            if(pNewSur)
            {
                SDL_Rect dst;
                dst.x = 0;
                dst.y = 0;
                SDL_BlitSurface(pShadow, nullptr, pNewSur, &dst);
                dst.x = 0;
                dst.y = 1;
                SDL_BlitSurface(pShadow, nullptr, pNewSur, &dst);
                dst.x = 0;
                dst.y = 2;
                SDL_BlitSurface(pShadow, nullptr, pNewSur, &dst);
                dst.x = 1;
                dst.y = 0;
                SDL_BlitSurface(pShadow, nullptr, pNewSur, &dst);
                dst.x = 1;
                dst.y = 2;
                SDL_BlitSurface(pShadow, nullptr, pNewSur, &dst);
                dst.x = 2;
                dst.y = 0;
                SDL_BlitSurface(pShadow, nullptr, pNewSur, &dst);
                dst.x = 2;
                dst.y = 1;
                SDL_BlitSurface(pShadow, nullptr, pNewSur, &dst);
                dst.x = 2;
                dst.y = 2;
                SDL_BlitSurface(pShadow, nullptr, pNewSur, &dst);

                dst.x = 1;
                dst.y = 1;
                SDL_BlitSurface(pSur, nullptr, pNewSur, &dst);

                TextCharOutlineCache[ch] = pNewSur;
                success = 1;
            }
            SDL_DestroySurface(pShadow);
        }
    }
    
    if(success)
        return true;
    
    SDL_Log("Error creating image for character %d\n", ch);

    if(pSur)
    {
        SDL_DestroySurface(pSur);
        TextCharCache.erase(ch);
    }

    return false;
}

bool CKZText::AddCharSet(std::string epiccharset)
{
    int i = 0;
    while(i < epiccharset.size())
    {
        const unsigned char * p_ch = ((const unsigned char *)epiccharset.c_str())+i;

        char utf8size =
        !(*p_ch & 128) ? 1 :
        (
            ((*p_ch & (64 | 32)) == 64) ? 2 :
            (
                ((*p_ch & (64 | 32 | 16)) == (64 | 32)) ? 3 :
                (
                    ((*p_ch & (64 | 32 | 16 | 8)) == (64 | 32 | 16)) ? 4 : 0
                )
            )
        );

        if(!utf8size)
        {
            SDL_Log("invalid utf8 character %d (next bytes %d %d %d)\n", p_ch[0], p_ch[1], p_ch[2], p_ch[3]);
            continue;
        }

        AddCharacter(p_ch);
        
        i += utf8size;
    }

    return true;
}

void CKZText::SetCurrentColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
    CurrentColor.r = r;
    CurrentColor.g = g;
    CurrentColor.b = b;
    CurrentColor.a = a;
}

void CKZText::RenderText(std::string epictexttorender, int x, int y)
{
    int offsetx = 0;
    int offsety = 0;
    Uint32 utf8char = 0;
    for (int i = 0; i < epictexttorender.size(); i++)
    {
        unsigned char ch = epictexttorender[i];
        if(ch == '\r')
        {
            continue;
        }

        if(ch == ' ')
        {
            offsetx += 10;
            continue;
        }

        if(ch == '\t')
        {
            offsetx += 40;
            continue;
        }

        if(ch == '\n')
        {
            offsetx = 0;
            offsety += 22;
            continue;
        }

        unsigned char * p_ch = (unsigned char *)&ch;

        char utf8size =
        !(*p_ch & 128) ? 1 :
        (
            ((*p_ch & (64 | 32)) == 64) ? 2 :
            (
                ((*p_ch & (64 | 32 | 16)) == (64 | 32)) ? 3 :
                (
                    ((*p_ch & (64 | 32 | 16 | 8)) == (64 | 32 | 16)) ? 4 : 0
                )
            )
        );

        // 'i' can get out of bounds because this, dont use it after this code
        utf8char = 0;
        utf8char = ch << 24;
        if(utf8size > 1)
        {
            i++;
            utf8char |= ((unsigned char)epictexttorender[i]) << 16;
            if(utf8size > 2)
            {
                i++;
                utf8char |= ((unsigned char)epictexttorender[i]) << 8;
                if(utf8size > 3)
                {
                    i++;
                    utf8char |= ((unsigned char)epictexttorender[i]);
                }
            }
        }
        

        SDL_Surface * pCh = nullptr;
        try
        {
            if(Outline)
            {
                pCh = TextCharOutlineCache.at(utf8char);
            }
            else
            {
                pCh = TextCharCache.at(utf8char);
            }
        }
        catch(const std::exception& e)
        {
            std::cout << e.what() << '\n';
            SDL_Log("Tried to render unknown character '%u' (size: %d)\n", utf8char, utf8size);
        }
        
        if(pCh)
        {
            SDL_Rect dst;
            dst.x = x + offsetx;
            dst.y = y + offsety;
            SDL_BlitSurface(pCh, nullptr, KZWindow.pMainSurface, &dst);
            offsetx += pCh->w;
        }
    }
}

void CKZText::CalculateRenderSize(std::string epictexttorender, int * out_x, int * out_y)
{
    int biggestx = 0;
    int offsetx = 0;
    int offsety = 0;
    Uint32 utf8char = 0;
    for (int i = 0; i < epictexttorender.size(); i++)
    {
        unsigned char ch = epictexttorender[i];
        if(ch == '\r')
        {
            continue;
        }

        if(ch == ' ')
        {
            offsetx += 10;
            if(biggestx < offsetx)
                biggestx = offsetx;
            continue;
        }

        if(ch == '\t')
        {
            offsetx += 40;
            if(biggestx < offsetx)
                biggestx = offsetx;
            continue;
        }

        if(ch == '\n')
        {
            if(biggestx < offsetx)
                biggestx = offsetx;
            offsetx = 0;
            offsety += 22;
            continue;
        }


        unsigned char * p_ch = (unsigned char *)&ch;

        char utf8size =
        !(*p_ch & 128) ? 1 :
        (
            ((*p_ch & (64 | 32)) == 64) ? 2 :
            (
                ((*p_ch & (64 | 32 | 16)) == (64 | 32)) ? 3 :
                (
                    ((*p_ch & (64 | 32 | 16 | 8)) == (64 | 32 | 16)) ? 4 : 0
                )
            )
        );

        // 'i' can get out of bounds because this, dont use it after this code
        utf8char = 0;
        utf8char = ch << 24;
        if(utf8size > 1)
        {
            i++;
            utf8char |= ((unsigned char)epictexttorender[i]) << 16;
            if(utf8size > 2)
            {
                i++;
                utf8char |= ((unsigned char)epictexttorender[i]) << 8;
                if(utf8size > 3)
                {
                    i++;
                    utf8char |= ((unsigned char)epictexttorender[i]);
                }
            }
        }
        

        SDL_Surface * pCh = nullptr;
        try
        {
            if(Outline)
            {
                pCh = TextCharOutlineCache.at(utf8char);
            }
            else
            {
                pCh = TextCharCache.at(utf8char);
            }
        }
        catch(const std::exception& e)
        {
            std::cout << e.what() << '\n';
            SDL_Log("Tried to get character '%u'\n", utf8char);
        }
        
        if(pCh)
        {
            offsetx += pCh->w;
            if(biggestx < offsetx)
                biggestx = offsetx;
        }
    }

    if(out_x)
    {
        *out_x = biggestx;
    }

    if(out_y)
    {
        *out_y = offsety + 22; // do like a '\n'
    }
}
