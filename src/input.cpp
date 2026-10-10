/*
    KZFramework
    Copyright (C) Benjamín Gajardo

    This program is shared under the PLUSKAIZO LICENSE, you should have received
    a copy of the license along with the program.
*/

#include "input.h"
#include "window.h"

#include <cstdio>

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
    
    /*case SDL_EVENT_MOUSE_MOTION:
        InputState.MouseX = QueuedInput.MouseX = e.motion.x;
        InputState.MouseY = QueuedInput.MouseY = e.motion.y;
        break;*/

    case SDL_EVENT_TEXT_INPUT:
        snprintf(InputState.LastTextInput, sizeof(InputState.LastTextInput), "%s", e.text.text);
        snprintf(QueuedInput.LastTextInput, sizeof(QueuedInput.LastTextInput), "%s", e.text.text);
        break;

    case SDL_EVENT_KEY_DOWN:
    case SDL_EVENT_KEY_UP:
        switch (e.key.scancode)
        {
        case SDL_SCANCODE_UP:
            if(e.type == SDL_EVENT_KEY_DOWN)
                InputState.Up = true;
            QueuedInput.Up = e.key.down;
            break;

        case SDL_SCANCODE_DOWN:
            if(e.type == SDL_EVENT_KEY_DOWN)
                InputState.Down = true;
            QueuedInput.Down = e.key.down;
            break;

        case SDL_SCANCODE_LEFT:
            if(e.type == SDL_EVENT_KEY_DOWN)
                InputState.Left = true;
            QueuedInput.Left = e.key.down;
            break;

        case SDL_SCANCODE_RIGHT:
            if(e.type == SDL_EVENT_KEY_DOWN)
                InputState.Right = true;
            QueuedInput.Right = e.key.down;
            break;

        case SDL_SCANCODE_Z:
            if(e.type == SDL_EVENT_KEY_DOWN)
                InputState.Jump = true;
            QueuedInput.Jump = e.key.down;
            break;

        case SDL_SCANCODE_S:
            if(e.type == SDL_EVENT_KEY_DOWN)
                InputState.Run = true;
            QueuedInput.Run = e.key.down;
            break;
        
        case SDL_SCANCODE_A:
            if(e.type == SDL_EVENT_KEY_DOWN)
                InputState.AltJump = true;
            QueuedInput.AltJump = e.key.down;
            break;

        case SDL_SCANCODE_RETURN:
            if(e.type == SDL_EVENT_KEY_DOWN)
                InputState.Enter = true;
            QueuedInput.Enter = e.key.down;
            break;

        case SDL_SCANCODE_ESCAPE:
            if(e.type == SDL_EVENT_KEY_DOWN)
                InputState.Pause = true;
            QueuedInput.Pause = e.key.down;
            break;

        case SDL_SCANCODE_BACKSPACE:
            if(e.type == SDL_EVENT_KEY_DOWN)
                InputState.BackSpace = true;
            QueuedInput.BackSpace = e.key.down;
            break;

        case SDL_SCANCODE_LSHIFT:
        case SDL_SCANCODE_RSHIFT:
            if(e.type == SDL_EVENT_KEY_DOWN)
                InputState.Shift = true;
            QueuedInput.Shift = e.key.down;
            break;

        default:
            break;
        }
        break;
    
    default:
        break;
    }

    float mouseX, mouseY;
    SDL_GetGlobalMouseState(&mouseX, &mouseY);

    int windowX, windowY;
    SDL_GetWindowPosition(KZWindow.pMainWindow, &windowX, &windowY);

    int w, h;
    SDL_GetWindowSizeInPixels(KZWindow.pMainWindow, &w, &h);

    float scaleX = (float)w / KZWindow.Width;
    float scaleY = (float)h / KZWindow.Height;
    float scale  = (scaleX < scaleY) ? scaleX : scaleY;

    float destW = KZWindow.Width * scale;
    float destH = KZWindow.Height * scale;

    float destX = (w - destW) / 2.0f;
    float destY = (h - destH) / 2.0f;

    // Mouse position relative to the window
    float localMouseX = mouseX - windowX;
    float localMouseY = mouseY - windowY;

    // Is the mouse inside the actual rendered game area?
    if (localMouseX >= destX &&
        localMouseX <  destX + destW &&
        localMouseY >= destY &&
        localMouseY <  destY + destH)
    {
        // Convert from scaled window coordinates
        // back to game coordinates.
        float gameMouseX = (localMouseX - destX) / scale;
        float gameMouseY = (localMouseY - destY) / scale;

        InputState.MouseX = QueuedInput.MouseX = gameMouseX;
        InputState.MouseY = QueuedInput.MouseY = gameMouseY;
    }
    else
    {
        // Ignore clicks outside the game area
        InputState.MouseClick = QueuedInput.MouseClick = false;
        InputState.MouseRightClick = QueuedInput.MouseRightClick = false;
    }
}

void CKZInput::UpdateInput()
{
    memcpy(&PreviousInputState, &InputState, sizeof(PreviousInputState));
    memcpy(&InputState, &QueuedInput, sizeof(InputState));
    InputState.LastTextInput[0] = QueuedInput.LastTextInput[0] = '\0';
}

void CKZInput::SetTextInputMode(bool mode)
{
    if(mode)
    {
        SDL_StartTextInput(KZWindow.pMainWindow);
    }
    else if(SDL_TextInputActive(KZWindow.pMainWindow))
    {
        SDL_StopTextInput(KZWindow.pMainWindow);
    }
}

Uint64 CKZInput::EvilRandom()
{
    #define EVIL_SEED_SIZE 16
    static Uint8 evilseed[EVIL_SEED_SIZE] = {0};
    static union
    {
        Uint64 val = 0;
        Uint8 bytes[8];
    } lastres;
    static Uint8 lastind = 0;

    lastind++;

    evilseed[0] = (evilseed[0] << 1) | (
        KZInput.InputState.Left << 0 |
        KZInput.InputState.Right << 1 |
        KZInput.InputState.Up << 2 |
        KZInput.InputState.Down << 3 |
        KZInput.InputState.Jump << 4 |
        KZInput.InputState.AltJump << 5 |
        KZInput.InputState.AltRun << 6 |
        KZInput.InputState.Run << 7 |
        (
            evilseed[15] & 1 ? 1 : 0
        )
    );
    evilseed[1] += ((1 << 2) & evilseed[0]) ? 1 : 2;
    evilseed[2] += 2 * (evilseed[7] + 7 * evilseed[0]);
    evilseed[3] -= 2 * (evilseed[11] - 7 * evilseed[0]);
    evilseed[4] += KZInput.InputState.Left + evilseed[0];
    evilseed[5] += KZInput.InputState.Right + evilseed[1];
    evilseed[6] += (KZInput.InputState.Jump + 1) * (evilseed[5] | 1);
    evilseed[7] -= KZInput.InputState.Up | evilseed[0];
    evilseed[8] += KZInput.InputState.Down | 2 | evilseed[1];
    evilseed[9] *= (KZInput.InputState.Run + 1) * (evilseed[7] | 1);
    evilseed[10] = KZInput.InputState.AltJump + (evilseed[9] + 1);
    evilseed[11] *= evilseed[8];
    evilseed[12] += KZInput.InputState.AltRun * evilseed[0];
    evilseed[13] -= KZInput.InputState.Left ? 5 * evilseed[0] : -5 * evilseed[0];
    evilseed[14] -= KZInput.InputState.Right ? -5 * evilseed[0] : 5 * evilseed[0];
    evilseed[15] ^= 0xFE & ((1 << 4) & evilseed[0]) ? 1 : 16;

    for(int i = 0; i < 8; i++)
    {
        lastres.bytes[i] = evilseed[(lastind + i) % EVIL_SEED_SIZE];
    }

    if(lastres.bytes[0] & (1 << (lastres.val % 8)))
    {
        evilseed[15] |= 1 << 8;
    }
    else
    {
        evilseed[15] &= 0xFE;
    }

    lastres.val &= 0xFF ^ (1 << (lastind % 8));

    return lastres.val;

    #undef EVIL_SEED_SIZE
}
