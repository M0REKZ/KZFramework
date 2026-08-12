#include "input.h"

CKZInput KZInput;

void CKZInput::ResetInput()
{
    memset(&InputState, 0, sizeof(InputState));
    memset(&QueuedInput, 0, sizeof(QueuedInput));
}

void CKZInput::HandleInput(SDL_Event e)
{
    switch (e.type)
    {
    case SDL_EVENT_MOUSE_BUTTON_UP:
        if(e.button.button == SDL_BUTTON_LEFT)
            QueuedInput.MouseClick = e.button.down;
        if(e.button.button == SDL_BUTTON_RIGHT)
            QueuedInput.MouseRightClick = e.button.down;
        break;
    case SDL_EVENT_MOUSE_BUTTON_DOWN:
        if(e.button.button == SDL_BUTTON_LEFT)
        {
            InputState.MouseClick = e.button.down;
            QueuedInput.MouseClick = e.button.down;
        }
        if(e.button.button == SDL_BUTTON_RIGHT)
        {
            InputState.MouseRightClick = e.button.down;
            QueuedInput.MouseRightClick = e.button.down;
        }
        break;
    
    case SDL_EVENT_MOUSE_MOTION:
        InputState.MouseX = QueuedInput.MouseX = e.motion.x;
        InputState.MouseY = QueuedInput.MouseY = e.motion.y;
        break;

    case SDL_EVENT_KEY_DOWN:
    case SDL_EVENT_KEY_UP:
        switch (e.key.key)
        {
        case SDLK_UP:
            if(e.type == SDL_EVENT_KEY_DOWN)
                InputState.Up = true;
            QueuedInput.Up = e.key.down;
            break;

        case SDLK_DOWN:
            if(e.type == SDL_EVENT_KEY_DOWN)
                InputState.Down = true;
            QueuedInput.Down = e.key.down;
            break;

        case SDLK_LEFT:
            if(e.type == SDL_EVENT_KEY_DOWN)
                InputState.Left = true;
            QueuedInput.Left = e.key.down;
            break;

        case SDLK_RIGHT:
            if(e.type == SDL_EVENT_KEY_DOWN)
                InputState.Right = true;
            QueuedInput.Right = e.key.down;
            break;

        case SDLK_Z:
            if(e.type == SDL_EVENT_KEY_DOWN)
                InputState.Jump = true;
            QueuedInput.Jump = e.key.down;
            break;

        case SDLK_S:
            if(e.type == SDL_EVENT_KEY_DOWN)
                InputState.Run = true;
            QueuedInput.Run = e.key.down;
            break;
        
        default:
            break;
        }
        break;
    
    default:
        break;
    }
}

void CKZInput::UpdateInput()
{
    memcpy(&InputState, &QueuedInput, sizeof(InputState));
}

Uint64 CKZInput::EvilRandom()
{
    static Uint8 evilseed[16] = {0};
    static union
    {
        Uint64 val = 0;
        Uint8 bytes[8];
    } lastres;
    static Uint8 lastind = 0;

    lastind++;

    evilseed[0] += (evilseed[14] + 1);
    evilseed[1] -= (evilseed[13] - 1);
    evilseed[2] += 2 * (evilseed[7] + 7);
    evilseed[3] -= 2 * (evilseed[11] - 7);
    evilseed[4] += KZInput.InputState.Left + evilseed[0];
    evilseed[5] += KZInput.InputState.Right + evilseed[1];
    evilseed[6] += (KZInput.InputState.Jump + 1) * (evilseed[5] | 1);
    evilseed[7] -= KZInput.InputState.Up | evilseed[0];
    evilseed[8] += KZInput.InputState.Down | 2 | evilseed[1];
    evilseed[9] *= (KZInput.InputState.Run + 1) * (evilseed[7] | 1);
    evilseed[10] = KZInput.InputState.AltJump + (evilseed[9] + 1);
    evilseed[11] *= evilseed[8];
    evilseed[12] += KZInput.InputState.AltRun;
    evilseed[13] -= KZInput.InputState.Left ? 5 : -5;
    evilseed[14] -= KZInput.InputState.Right ? -5 : 5;
    evilseed[15] ^= 0xFE;

    for(int i = 0;i < 8; i++)
    {
        lastres.bytes[i] = evilseed[(lastind + i) % sizeof(evilseed)];
    }

    if(lastres.bytes[0] & (1 << (lastres.val % 8)))
    {
        evilseed[15] |= 1 << 8;
    }

    return lastres.val;
}
