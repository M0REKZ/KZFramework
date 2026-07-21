#include <SDL3/SDL.h>

class CKZWindow
{
public:
    //the window
    SDL_Window * pMainWindow = nullptr;

    //render texture to the window
    SDL_Renderer * pMainWindowRenderer = nullptr;

    //the surface where we want to draw
    SDL_Surface * pMainSurface = nullptr;

    //we need to convert the surface to texture before drawing
    SDL_Texture * pMainTexture = nullptr;

    const int Width = 256;
    const int Height = 244;

    bool Init();
    void Destroy();

    void UpdateWindow();

    inline Uint32 ConverToWindowDrawColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a)
    {
        return SDL_MapSurfaceRGBA(pMainSurface, r, g, b, a);
    }
};

extern CKZWindow KZWindow;
