#ifndef PLUSKAIZO_INPUT_H
#define PLUSKAIZO_INPUT_H

#include <SDL3/SDL_events.h>

class CKZInput
{
public:
    CKZInput()
    {
        ResetInput();
    }
    void ResetInput();
    void HandleInput(SDL_Event e);
    void UpdateInput();

    struct SState
    {
        Uint8 Up;
        Uint8 Down;
        Uint8 Left;
        Uint8 Right;
        Uint8 Jump;
        Uint8 AltJump;
        Uint8 Run;
        Uint8 AltRun;

        float MouseX;
        float MouseY;
        Uint8 MouseClick;
        Uint8 MouseRightClick;
    };
    
    SState InputState;
private:
    SState QueuedInput;
};

extern CKZInput KZInput;

#endif
