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

#include "input.h"
#include "window.h"
#include "scripts.h"
#include "main.h"
#include "crossplatform.h"

#include <SDL3/SDL.h>

#ifdef __EMSCRIPTEN__
    #include <emscripten.h>
    #include <emscripten/html5.h>
#endif

CKZCrossPlatform KZCrossPlatform;

#ifdef __EMSCRIPTEN__

    EM_JS(void, InitSavePathEmscripten, (const char* savepath), {
        const mountPath = UTF8ToString(savepath);

        Asyncify.handleSleep(function(wakeUp) {
            try {
                FS.mkdir(mountPath);
            } catch (e) {}

            try {
                FS.mount(IDBFS, {}, mountPath);
            } catch (e) {}

            FS.syncfs(true, function(err) {
                if (err)
                    console.error(err);

                wakeUp();
            });
        });
    });

    EM_JS(void, FlushSavePathEmscripten, (), {
        FS.syncfs(false, function(err) {
            if (err)
                console.error(err);
        });
    });

    bool SetMainLoopPropertiesEmscripten = false;
#endif

    bool CKZCrossPlatform::InitSavePath(const char *path)
    {
    #ifdef __EMSCRIPTEN__
        InitSavePathEmscripten(path);
        return true;
    #else
        return true;
    #endif
}

bool CKZCrossPlatform::FlushSavePath()
{
    #ifdef __EMSCRIPTEN__
        FlushSavePathEmscripten();
        return true;
    #else
        return true;
    #endif
}

SDL_Event sdlevent;

void CKZCrossPlatform::GameLoop()
{
    #ifdef __EMSCRIPTEN__
        if(!SetMainLoopPropertiesEmscripten)
        {
            emscripten_set_main_loop_timing(EM_TIMING_SETTIMEOUT, 0);
            SetMainLoopPropertiesEmscripten = true;
        }
    #endif

    #ifndef __EMSCRIPTEN__
    while(running)
    {
    #endif
        KZInput.UpdateInput();
        while(SDL_PollEvent(&sdlevent))
        {
            if(sdlevent.type == SDL_EVENT_QUIT)
            {
                running = false;
            }
            KZInput.HandleInput(sdlevent);
        }
        #ifdef __ANDROID__
            KZCrossPlatform.UpdateTouchControls();
            KZCrossPlatform.HandleTouchInput();
        #endif

        if(!KZScripts.RunMain())
        {
            running = false;
        }
        else
        {
            KZWindow.UpdateWindow();
            KZWindow.DoTargetDelay();
        }
    #ifndef __EMSCRIPTEN__
    }
    #endif

    if(!running)
    {
        #ifdef __EMSCRIPTEN__
            destroy_everything();
            emscripten_cancel_main_loop();
        #endif
    }
}

void CKZCrossPlatform::DoMainLoop()
{
    #ifdef __EMSCRIPTEN__
        emscripten_set_main_loop(KZCrossPlatform.GameLoop, 0, true);
    #else
        GameLoop();
    #endif
}

#ifdef __ANDROID__

SDL_Surface * pDrawControlsSurface = nullptr;
SDL_Texture * pDrawControlsTexture = nullptr;
bool FirstRun = true;

SDL_Rect LeftButton, RightButton, UpButton, DownButton, JumpButton, AltJumpButton, RunButton, AltRunButton, PauseButton;

#endif

void CKZCrossPlatform::UpdateTouchControls()
{
    #ifdef __ANDROID__
        int w, h;
        if(!SDL_GetWindowSizeInPixels(KZWindow.pMainWindow, &w, &h))
            return;

        //simple way to detect if phone is landscape so we put the buttons in a better position
        bool Landscape = w > h;

        int ButtonsSizeY = Landscape ? 150 : 130;
        int ButtonsStartX = Landscape ? 75 : 25;
        int ButtonsStartY = (h - ((Landscape ? 75 : 256) + ButtonsSizeY * 2)) + ButtonsSizeY / 2;

        //Left
        LeftButton = {ButtonsStartX, ButtonsStartY, ButtonsSizeY + ButtonsSizeY/2, ButtonsSizeY};
        RightButton = {LeftButton.x + LeftButton.w + 6, ButtonsStartY, ButtonsSizeY + ButtonsSizeY/2, ButtonsSizeY};
        DownButton = {LeftButton.x + (LeftButton.w + 6 * 2) / 2, ButtonsStartY + ButtonsSizeY + 6, ButtonsSizeY + ButtonsSizeY/2 + 3 * 2, ButtonsSizeY / 3 * 2};
        UpButton = DownButton;
        UpButton.y = LeftButton.y - (UpButton.h + 6);

        //UpRight
        PauseButton = {w - (ButtonsSizeY + ButtonsStartX), Landscape ? 50 : 125, ButtonsSizeY, ButtonsSizeY};

        //Right
        JumpButton = {w - (int)(ButtonsSizeY * 1.f + ButtonsStartX), ButtonsStartY, (int)(ButtonsSizeY * 1.f), (int)(ButtonsSizeY * 1.5f)};
        AltJumpButton = JumpButton;
        AltJumpButton.y -= JumpButton.h + 6;
        RunButton = JumpButton;
        RunButton.x -= JumpButton.w + 6;
        RunButton.y -= (JumpButton.h + 6) / 2;
    #endif
}

void CKZCrossPlatform::HandleTouchInput()
{
    #ifdef __ANDROID__
        int devicecount = 0;
        SDL_TouchID * touchids = SDL_GetTouchDevices(&devicecount);
        
        if(!touchids)
            return;

        int w, h;
        if(!SDL_GetWindowSizeInPixels(KZWindow.pMainWindow, &w, &h))
            return;

        //Release keys
        bool left_pressed = false;
        bool right_pressed = false;
        bool down_pressed = false;
        bool up_pressed = false;
        bool z_pressed = false;
        bool escape_pressed = false;
        bool altjump_pressed = false;

        static bool run_pressed = false;
        static bool run_waiting_release = false;
        bool finger_in_run = false;

        //Save pressed keys
        for(int j = 0; j < devicecount; ++j)
        {
            int count = 0;
            SDL_Finger **fingers = SDL_GetTouchFingers(touchids[j], &count);

            for(int i = 0; i < count; i ++)
            {
                SDL_Point TouchedPos = {(int)(fingers[i]->x * w), (int)(fingers[i]->y * h)};

                if(SDL_PointInRect(&TouchedPos, &LeftButton))
                    left_pressed = true; 
                else if(SDL_PointInRect(&TouchedPos, &RightButton))
                    right_pressed = true;
                else if(SDL_PointInRect(&TouchedPos, &DownButton))
                    down_pressed = true; 
                else if(SDL_PointInRect(&TouchedPos, &UpButton))
                    up_pressed = true; 
                else if(SDL_PointInRect(&TouchedPos, &JumpButton))
                    z_pressed = true;
                else if(SDL_PointInRect(&TouchedPos, &PauseButton))
                    escape_pressed = true;
                else if(SDL_PointInRect(&TouchedPos, &AltJumpButton))
                    altjump_pressed = true;

                if(SDL_PointInRect(&TouchedPos, &RunButton))
                {
                    finger_in_run = true;
                    if(!run_waiting_release)
                    {
                        run_pressed = !run_pressed;
                        run_waiting_release = true;
                    }
                }

            }
        }

        if(!finger_in_run)
        {
            run_waiting_release = false;
        }

        //Queue released keys
        if(left_pressed)
            KZInput.InputState.Left = true;
        KZInput.QueuedInput.Left = left_pressed;
        
        if(right_pressed)
            KZInput.InputState.Right = true;
        KZInput.QueuedInput.Right = right_pressed;

        if(down_pressed)
            KZInput.InputState.Down = true;
        KZInput.QueuedInput.Down = down_pressed;

        if(up_pressed)
            KZInput.InputState.Up = true;
        KZInput.QueuedInput.Up = up_pressed;

        if(z_pressed)
            KZInput.InputState.Jump = true;
        KZInput.QueuedInput.Jump = z_pressed;

        if(escape_pressed)
            KZInput.InputState.Pause = true;
        KZInput.QueuedInput.Pause = escape_pressed;

        if(altjump_pressed)
            KZInput.InputState.AltJump = true;
        KZInput.QueuedInput.AltJump = altjump_pressed;

        if(run_pressed)
            KZInput.InputState.Run = true;
        KZInput.QueuedInput.Run = run_pressed;
    #endif
}

void CKZCrossPlatform::DrawBasicTouchButtons()
{
    #ifdef __ANDROID__
        int w, h;
        if(SDL_GetWindowSizeInPixels(KZWindow.pMainWindow, &w, &h))
        {
            static int prev_window_w = -1, prev_window_h = -1;
            if(
                !pDrawControlsSurface ||
                !pDrawControlsTexture ||
                prev_window_w != w ||
                prev_window_h != h
            )
            {

                if(pDrawControlsSurface || FirstRun)
                {
                    if(pDrawControlsSurface)
                        SDL_DestroySurface(pDrawControlsSurface);
                    pDrawControlsSurface = SDL_CreateSurface(w, h, SDL_PIXELFORMAT_RGBA8888);
                }

                if(!pDrawControlsSurface)
                    return;

                if(pDrawControlsTexture || FirstRun)
                {
                    if(pDrawControlsTexture)
                        SDL_DestroyTexture(pDrawControlsTexture);
                    pDrawControlsTexture = SDL_CreateTexture(KZWindow.pMainWindowRenderer, SDL_PIXELFORMAT_RGBA8888,
                    SDL_TEXTUREACCESS_STREAMING, w, h);
                }

                if(!pDrawControlsTexture)
                    return;
                
                SDL_SetTextureBlendMode(pDrawControlsTexture, SDL_BLENDMODE_BLEND);
                SDL_SetTextureScaleMode(pDrawControlsTexture, SDL_SCALEMODE_NEAREST);

                prev_window_w = w;
                prev_window_h = h;
            }

            SDL_ClearSurface(pDrawControlsSurface, 0, 0, 0, 0);

            Uint32 buttoncolor;

            if(KZInput.InputState.Left)
                buttoncolor = SDL_MapSurfaceRGBA(pDrawControlsSurface, 150, 150, 150, 100);
            else
                buttoncolor = SDL_MapSurfaceRGBA(pDrawControlsSurface, 255, 255, 255, 100);
            SDL_FillSurfaceRect(pDrawControlsSurface, &LeftButton, buttoncolor);
            
            if(KZInput.InputState.Right)
                buttoncolor = SDL_MapSurfaceRGBA(pDrawControlsSurface, 150, 150, 150, 100);
            else
                buttoncolor = SDL_MapSurfaceRGBA(pDrawControlsSurface, 255, 255, 255, 100);
            SDL_FillSurfaceRect(pDrawControlsSurface, &RightButton, buttoncolor);

            if(KZInput.InputState.Down)
                buttoncolor = SDL_MapSurfaceRGBA(pDrawControlsSurface, 150, 150, 150, 100);
            else
                buttoncolor = SDL_MapSurfaceRGBA(pDrawControlsSurface, 255, 255, 255, 100);
            SDL_FillSurfaceRect(pDrawControlsSurface, &DownButton, buttoncolor);

            if(KZInput.InputState.Up)
                buttoncolor = SDL_MapSurfaceRGBA(pDrawControlsSurface, 150, 150, 150, 100);
            else
                buttoncolor = SDL_MapSurfaceRGBA(pDrawControlsSurface, 255, 255, 255, 100);
            SDL_FillSurfaceRect(pDrawControlsSurface, &UpButton, buttoncolor);

            if(KZInput.InputState.Jump)
                buttoncolor = SDL_MapSurfaceRGBA(pDrawControlsSurface, 150, 150, 150, 100);
            else
                buttoncolor = SDL_MapSurfaceRGBA(pDrawControlsSurface, 255, 255, 255, 100);
            SDL_FillSurfaceRect(pDrawControlsSurface, &JumpButton, buttoncolor);

            if(KZInput.InputState.AltJump)
                buttoncolor = SDL_MapSurfaceRGBA(pDrawControlsSurface, 150, 150, 150, 100);
            else
                buttoncolor = SDL_MapSurfaceRGBA(pDrawControlsSurface, 255, 255, 255, 100);
            SDL_FillSurfaceRect(pDrawControlsSurface, &AltJumpButton, buttoncolor);

            if(KZInput.InputState.Run)
                buttoncolor = SDL_MapSurfaceRGBA(pDrawControlsSurface, 150, 255, 150, 100);
            else
                buttoncolor = SDL_MapSurfaceRGBA(pDrawControlsSurface, 255, 150, 150, 100);
            SDL_FillSurfaceRect(pDrawControlsSurface, &RunButton, buttoncolor);

            if(KZInput.InputState.AltRun)
                buttoncolor = SDL_MapSurfaceRGBA(pDrawControlsSurface, 150, 150, 150, 100);
            else
                buttoncolor = SDL_MapSurfaceRGBA(pDrawControlsSurface, 255, 255, 255, 100);
            SDL_FillSurfaceRect(pDrawControlsSurface, &AltRunButton, buttoncolor);

            if(KZInput.InputState.Pause)
                buttoncolor = SDL_MapSurfaceRGBA(pDrawControlsSurface, 150, 150, 150, 100);
            else
                buttoncolor = SDL_MapSurfaceRGBA(pDrawControlsSurface, 255, 255, 255, 100);
            SDL_FillSurfaceRect(pDrawControlsSurface, &PauseButton, buttoncolor);

            buttoncolor = SDL_MapSurfaceRGBA(pDrawControlsSurface, 0, 0, 0, 100);

            //MOAHAHAHAHAHAA
            #define EpicKZEvilRectangleColor(surface, rect, color) \
            {   \
                SDL_Rect tempthing = rect;  \
                tempthing.w = 1;    \
                SDL_FillSurfaceRect(surface, &tempthing, color);    \
                tempthing.x += rect.w - 1;  \
                SDL_FillSurfaceRect(surface, &tempthing, color);    \
                tempthing.x = rect.x;   \
                tempthing.w = rect.w;   \
                \
                tempthing.h = 1;    \
                SDL_FillSurfaceRect(surface, &tempthing, color);    \
                tempthing.y += rect.h - 1;  \
                SDL_FillSurfaceRect(surface, &tempthing, color);    \
            }

            //Normal buttons
            EpicKZEvilRectangleColor(pDrawControlsSurface, LeftButton, buttoncolor);
            EpicKZEvilRectangleColor(pDrawControlsSurface, RightButton, buttoncolor);
            EpicKZEvilRectangleColor(pDrawControlsSurface, DownButton, buttoncolor);
            EpicKZEvilRectangleColor(pDrawControlsSurface, UpButton, buttoncolor);
            EpicKZEvilRectangleColor(pDrawControlsSurface, JumpButton, buttoncolor);
            EpicKZEvilRectangleColor(pDrawControlsSurface, AltJumpButton, buttoncolor);
            EpicKZEvilRectangleColor(pDrawControlsSurface, RunButton, buttoncolor);
            EpicKZEvilRectangleColor(pDrawControlsSurface, AltRunButton, buttoncolor);

            //Pause button
            EpicKZEvilRectangleColor(pDrawControlsSurface, PauseButton, buttoncolor);
            buttoncolor = SDL_MapSurfaceRGBA(pDrawControlsSurface, 100, 100, 100, 100);
            SDL_Rect Decoration = {(PauseButton.x + PauseButton.w / 3) - (PauseButton.w / 6 + (PauseButton.w / 12)), PauseButton.y + PauseButton.h / 6, PauseButton.w / 3, (PauseButton.h / 3) * 2};
            SDL_FillSurfaceRect(pDrawControlsSurface, &Decoration, buttoncolor);
            Decoration.x = (PauseButton.x + ((PauseButton.w / 3) * 2) - (PauseButton.w / 6)) + (PauseButton.w / 12);
            SDL_FillSurfaceRect(pDrawControlsSurface, &Decoration, buttoncolor);

            SDL_UpdateTexture(pDrawControlsTexture, nullptr, pDrawControlsSurface->pixels, pDrawControlsSurface->pitch);
            SDL_RenderTexture(KZWindow.pMainWindowRenderer, pDrawControlsTexture, nullptr, nullptr);

            FirstRun = false;
        }
    #else
        return;
    #endif
}


