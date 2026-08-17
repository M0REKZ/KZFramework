
#include <string>
#include "window.h"
#include "text.h"
#include <iostream>

CKZText KZText;

bool CKZText::Init()
{
    if(!TTF_Init())
    {
        printf("ERROR INIT SDL_TTF: %s\n", SDL_GetError());
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

    std::string filename = "game/data/font/" + std::string(pfile) + ".ttf";

    TTF_Font * pFont = TTF_OpenFont(filename.c_str(), 20);

    if(pFont)
    {
        LoadedText = pFont;
        return true;
    }

    printf("ERROR: Could not load font %s!\n", filename.c_str());

    return false;
}

bool CKZText::InitText()
{
    bool res = true;

    //create basic cache
    char xd = '\0';
    for(xd = 0x21; xd < 0x7E; xd++)
    {
        if(!AddCharacter(xd))
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
}

bool CKZText::AddCharacter(char ch)
{
    if(!LoadedText)
        return false;

    if(ch == ' ')
        return false;

    SDL_Surface * pSur = TTF_RenderText_Blended(LoadedText, &ch, 1, CurrentColor);

    if(pSur)
    {
        TextCharCache[ch] = pSur;
        if(Outline)
        {
            SDL_Color color;
            color.r = 0;
            color.g = 0;
            color.b = 0;
            color.a = 255;
            SDL_Surface * pShadow = TTF_RenderText_Blended(LoadedText, &ch, 1, color);
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

                    SDL_DestroySurface(pSur);
                    TextCharCache[ch] = pNewSur;
                }
                SDL_DestroySurface(pShadow);
            }
        }
        return true;
    }
    
    printf("Error creating image for character %c\n", ch);

    return false;
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
    for(char ch : epictexttorender)
    {
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

        SDL_Surface * pCh = nullptr;
        try
        {
            pCh = TextCharCache.at(ch);
        }
        catch(const std::exception& e)
        {
            std::cout << e.what() << '\n';
            printf("Tried to render character '%c' '%d'\n", ch, (int)ch);
        }
        
        pCh = TextCharCache.at(ch);
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
