#ifndef PLUSKAIZO_TEXT_H
#define PLUSKAIZO_TEXT_H

#include <unordered_map>
#include <SDL3_ttf/SDL_ttf.h>

class CKZText
{
    //for now im just using C++ char
    std::unordered_map<char, SDL_Surface *> TextCharCache;
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
    bool AddCharacter(char ch);
    void SetCurrentColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a);
    void RenderText(std::string epictexttorender, int x, int y);
};

extern CKZText KZText;

#endif 
