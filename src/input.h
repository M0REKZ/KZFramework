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

    //Allows receiving chars from keyboard
    void SetTextInputMode(bool mode);

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

        Uint8 Enter;
        Uint8 BackSpace;
        Uint8 Pause;
        Uint8 Shift;
        char LastTextInput[128] = {'\0'};
    };
    
    SState InputState;
    SState PreviousInputState;

    SState QueuedInput;

    // probably the worst way of making """""random"""""
    Uint64 EvilRandom();
};

extern CKZInput KZInput;

#endif
