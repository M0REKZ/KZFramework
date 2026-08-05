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
