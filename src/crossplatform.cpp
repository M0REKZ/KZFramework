
#include "input.h"
#include "window.h"
#include "scripts.h"
#include "main.h"
#include "crossplatform.h"

#include <SDL3/SDL.h>

CKZCrossPlatform KZCrossPlatform;

#ifdef __EMSCRIPTEN__

    #include <emscripten.h>
    #include <emscripten/html5.h>

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
