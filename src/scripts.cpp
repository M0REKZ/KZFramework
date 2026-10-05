
#include <cmath>

extern "C"
{
    #include <lua.h>
    #include <lualib.h>
    #include <lauxlib.h>
}

#include "scripts.h"
#include "window.h"
#include "images.h"
#include "sounds.h"
#include "input.h"
#include "files.h"
#include "text.h"
#include "globals.h"
#include <SDL3/SDL.h>

lua_State * CKZScripts::MainLuaState = nullptr;

CKZScripts KZScripts;

static int KZWindowBindChangeWindowTitle(lua_State * ls)
{
    const char * title = luaL_checkstring(ls, 1);
    lua_pushboolean(ls, KZWindow.ChangeWindowTitle(title));
    return 1;
}

static int KZWindowBindDrawFillRect(lua_State * ls)
{
    int x_from = luaL_checkinteger(ls, 1);
    int y_from = luaL_checkinteger(ls, 2);
    int x_to = luaL_checkinteger(ls, 3);
    int y_to = luaL_checkinteger(ls, 4);
    Uint32 color = luaL_checkinteger(ls, 5);

    KZWindow.DrawFillRect(x_from, y_from, x_to, y_to, color);
    return 0;
}

static int KZWindowBindDrawLine(lua_State * ls)
{
    int x_from = luaL_checkinteger(ls, 1);
    int y_from = luaL_checkinteger(ls, 2);
    int x_to = luaL_checkinteger(ls, 3);
    int y_to = luaL_checkinteger(ls, 4);
    Uint32 color = luaL_checkinteger(ls, 5);

    KZWindow.DrawLine(x_from, y_from, x_to, y_to, color);
    return 0;
}

static int KZWindowBindDrawRect(lua_State * ls)
{
    int x_from = luaL_checkinteger(ls, 1);
    int y_from = luaL_checkinteger(ls, 2);
    int x_to = luaL_checkinteger(ls, 3);
    int y_to = luaL_checkinteger(ls, 4);
    Uint32 color = luaL_checkinteger(ls, 5);

    KZWindow.DrawRect(x_from, y_from, x_to, y_to, color);
    return 0;
}

static int KZWindowBindDrawEllipse(lua_State * ls)
{
    int cx = luaL_checkinteger(ls, 1);
    int cy = luaL_checkinteger(ls, 2);
    int rx = luaL_checkinteger(ls, 3);
    int ry = luaL_checkinteger(ls, 4);
    Uint32 color = luaL_checkinteger(ls, 5);

    KZWindow.DrawEllipse(cx, cy, rx, ry, color);
    return 0;
}

static int KZWindowBindDrawFillEllipse(lua_State * ls)
{
    int cx = luaL_checkinteger(ls, 1);
    int cy = luaL_checkinteger(ls, 2);
    int rx = luaL_checkinteger(ls, 3);
    int ry = luaL_checkinteger(ls, 4);
    Uint32 color = luaL_checkinteger(ls, 5);

    KZWindow.DrawFillEllipse(cx, cy, rx, ry, color);
    return 0;
}

static int KZWindowBindDelay(lua_State * ls)
{
    int ms = luaL_checkinteger(ls, 1);

    KZWindow.Delay(ms);
    return 0;
}

static int KZWindowBindConverToWindowDrawColor(lua_State * ls)
{
    Uint8 r = luaL_checkinteger(ls, 1);
    Uint8 g = luaL_checkinteger(ls, 2);
    Uint8 b = luaL_checkinteger(ls, 3);
    Uint8 a = luaL_checkinteger(ls, 4);

    lua_pushinteger(ls, KZWindow.ConverToWindowDrawColor(r, g, b, a));
    return 1;
}

static int KZWindow_GetWidth(lua_State * ls)
{
    lua_pushinteger(ls, KZWindow.Width);
    return 1;
}

static int KZWindow_GetHeight(lua_State * ls)
{
    lua_pushinteger(ls, KZWindow.Height);
    return 1;
}

static int KZWindow_SetTargetDelay(lua_State * ls)
{
    KZWindow.SetTargetDelay(luaL_checkinteger(ls, 1));
    return 0;
}

static int KZImagesBindLoadImage(lua_State * ls)
{
    unsigned int id = luaL_checkinteger(ls, 1);

    lua_pushinteger(ls, KZImages.LoadImage(id));
    return 1;
}

static int KZImagesBindUnloadImage(lua_State * ls)
{
    unsigned int id = luaL_checkinteger(ls, 1);

    KZImages.UnloadImage(id);
    return 0;
}

static int KZImagesBindRenderImage(lua_State * ls)
{
    unsigned int id = luaL_checkinteger(ls, 1);
    
    int x_from = (int)std::floor(luaL_optnumber(ls, 2, 0));
    int y_from = (int)std::floor(luaL_optnumber(ls, 3, 0));
    int w_from = (int)std::floor(luaL_optnumber(ls, 4, KZImages.GetImageWidth(id)));
    int h_from = (int)std::floor(luaL_optnumber(ls, 5, KZImages.GetImageHeight(id)));

    int x_to = (int)std::floor(luaL_checknumber(ls, 6));
    int y_to = (int)std::floor(luaL_checknumber(ls, 7));
    int w_to = (int)std::floor(luaL_optnumber(ls, 8, KZImages.GetImageWidth(id)));
    int h_to = (int)std::floor(luaL_optnumber(ls, 9, KZImages.GetImageHeight(id)));

    KZImages.RenderImage(id, x_from, y_from, w_from, h_from, x_to, y_to, w_to, h_to);
    return 0;
}

static int KZImagesBindRenderRotoZoom(lua_State * ls)
{
    unsigned int id = luaL_checkinteger(ls, 1);
    
    int x_from = (int)std::floor(luaL_optnumber(ls, 2, 0));
    int y_from = (int)std::floor(luaL_optnumber(ls, 3, 0));
    int w_from = (int)std::floor(luaL_optnumber(ls, 4, KZImages.GetImageWidth(id)));
    int h_from = (int)std::floor(luaL_optnumber(ls, 5, KZImages.GetImageHeight(id)));

    int cx_to = (int)std::floor(luaL_checknumber(ls, 6));
    int cy_to = (int)std::floor(luaL_checknumber(ls, 7));
    double zoomx = luaL_checknumber(ls, 8);
    double zoomy = luaL_checknumber(ls, 9);
    double angle = luaL_checknumber(ls, 10);

    KZImages.RenderRotoZoom(id, x_from, y_from, w_from, h_from, cx_to, cy_to, zoomx, zoomy, angle);
    return 0;
}

static int KZImagesBindKeepAreaAsImage(lua_State * ls)
{
    int x_from = (int)std::floor(luaL_checknumber(ls, 1));
    int y_from = (int)std::floor(luaL_checknumber(ls, 2));
    int w_from = (int)std::floor(luaL_checknumber(ls, 3));
    int h_from = (int)std::floor(luaL_checknumber(ls, 4));

    unsigned int id = luaL_checkinteger(ls, 5);

    KZImages.KeepAreaAsImage(x_from, y_from, w_from, h_from, id);
    return 0;
}

static int KZImagesBindUnloadAllImages(lua_State * ls)
{
    KZImages.UnloadAllImages();
    return 0;
}

static int KZImagesBindIsImageSlotBusy(lua_State * ls)
{
    unsigned int id = luaL_checkinteger(ls, 1);

    lua_pushboolean(ls, KZImages.IsImageSlotBusy(id));
    return 1;
}

static int KZWindow_GetImageWidth(lua_State * ls)
{
    unsigned int id = luaL_checkinteger(ls, 1);

    lua_pushinteger(ls, KZImages.GetImageWidth(id));
    return 1;
}

static int KZWindow_GetImageHeight(lua_State * ls)
{
    unsigned int id = luaL_checkinteger(ls, 1);
    
    lua_pushinteger(ls, KZImages.GetImageHeight(id));
    return 1;
}

static int KZSoundsBindLoadAudio(lua_State * ls)
{
    unsigned int id = luaL_checkinteger(ls, 1);
    bool predecode = lua_toboolean(ls, 2);

    lua_pushinteger(ls, KZSounds.LoadAudio(id, predecode));
    return 1;
}

static int KZSoundsBindLoadMusic(lua_State * ls)
{
    unsigned int id = luaL_checkinteger(ls, 1);

    lua_pushinteger(ls, KZSounds.LoadMusic(id));
    return 1;
}

static int KZSoundsBindLoadSoundEffect(lua_State * ls)
{
    unsigned int id = luaL_checkinteger(ls, 1);

    lua_pushinteger(ls, KZSounds.LoadSoundEffect(id));
    return 1;
}

static int KZSoundsBindIsSoundSlotBusy(lua_State * ls)
{
    unsigned int id = luaL_checkinteger(ls, 1);

    lua_pushboolean(ls, KZSounds.IsSoundSlotBusy(id));
    return 1;
}

static int KZSoundsBindPlayAudio(lua_State * ls)
{
    unsigned int id = luaL_checkinteger(ls, 1);
    bool loop = lua_toboolean(ls, 2);
    float vol = luaL_optnumber(ls, 3, 1.f);

    lua_pushboolean(ls, KZSounds.PlayAudio(id, loop, vol));
    return 1;
}

static int KZSoundsBindStopAudio(lua_State * ls)
{
    unsigned int id = luaL_checkinteger(ls, 1);

    KZSounds.StopAudio(id);
    return 0;
}

static int KZSoundsBindUnloadSound(lua_State * ls)
{
    unsigned int id = luaL_checkinteger(ls, 1);

    KZSounds.UnloadSound(id);
    return 0;
}

static int KZSoundsBindUnloadAllSounds(lua_State * ls)
{
    KZSounds.UnloadAllSounds();
    return 0;
}

static int KZInput_GetInputState(lua_State * ls)
{
    CKZInput::SState * pInputState = &KZInput.InputState;
    const char * pArg = luaL_optstring(ls, 1, nullptr);
    if(pArg && pArg[0] == 'p')
    {
        pInputState = &KZInput.PreviousInputState;
    }

    lua_newtable(ls);

    lua_pushboolean(ls, pInputState->Up);
    lua_setfield(ls, -2, "Up");

    lua_pushboolean(ls, pInputState->Down);
    lua_setfield(ls, -2, "Down");

    lua_pushboolean(ls, pInputState->Left);
    lua_setfield(ls, -2, "Left");

    lua_pushboolean(ls, pInputState->Right);
    lua_setfield(ls, -2, "Right");

    lua_pushboolean(ls, pInputState->Jump);
    lua_setfield(ls, -2, "Jump");

    lua_pushboolean(ls, pInputState->AltJump);
    lua_setfield(ls, -2, "AltJump");

    lua_pushboolean(ls, pInputState->Run);
    lua_setfield(ls, -2, "Run");

    lua_pushboolean(ls, pInputState->AltRun);
    lua_setfield(ls, -2, "AltRun");

    lua_pushnumber(ls, pInputState->MouseX);
    lua_setfield(ls, -2, "MouseX");

    lua_pushnumber(ls, pInputState->MouseY);
    lua_setfield(ls, -2, "MouseY");

    lua_pushboolean(ls, pInputState->MouseClick);
    lua_setfield(ls, -2, "MouseClick");

    lua_pushboolean(ls, pInputState->MouseRightClick);
    lua_setfield(ls, -2, "MouseRightClick");

    lua_pushboolean(ls, pInputState->Enter);
    lua_setfield(ls, -2, "Enter");

    lua_pushboolean(ls, pInputState->Pause);
    lua_setfield(ls, -2, "Pause");

    lua_pushboolean(ls, pInputState->Shift);
    lua_setfield(ls, -2, "Shift");

    lua_pushboolean(ls, pInputState->BackSpace);
    lua_setfield(ls, -2, "BackSpace");

    lua_pushstring(ls, pInputState->LastTextInput);
    lua_setfield(ls, -2, "LastTextInput");

    return 1;
}

static int KZInputBindEvilRandom(lua_State * ls)
{
    lua_pushinteger(ls, KZInput.EvilRandom());
    return 1;
}

static int KZInputBindSetTextInputMode(lua_State * ls)
{
    KZInput.SetTextInputMode(lua_toboolean(ls, 1));
    return 0;
}

static int KZFilesBindOpenFileForWriting(lua_State * ls)
{
    const char * pArg = luaL_checkstring(ls, 1);
    bool bin = lua_toboolean(ls, 2);
    if(pArg)
    {
        lua_pushboolean(ls, KZFiles.OpenFileForWriting(pArg, bin));
        return 1;
    }
    return 0;
}

static int KZFilesBindOpenFileForReading(lua_State * ls)
{
    const char * pArg = luaL_checkstring(ls, 1);
    bool bin = lua_toboolean(ls, 2);
    bool force_local = lua_toboolean(ls, 3);
    if(pArg)
    {
        lua_pushboolean(ls, KZFiles.OpenFileForReading(pArg, bin, force_local));
        return 1;
    }
    return 0;
}

static int KZFilesBindWriteLine(lua_State * ls)
{
    const char * pArg = luaL_checkstring(ls, 1);
    if(pArg)
    {
        lua_pushboolean(ls, KZFiles.WriteLine(pArg));
        return 1;
    }
    return 0;
}

static int KZFilesBindGetNextLine(lua_State * ls)
{
    lua_pushstring(ls, KZFiles.GetNextLine());
    return 1;
}

static int KZFilesBindCloseFile(lua_State * ls)
{
    KZFiles.CloseFile();
    return 0;
}

static int KZTextBindLoadText(lua_State * ls)
{
    const char * pArg = luaL_checkstring(ls, 1);
    if(pArg)
    {
        lua_pushboolean(ls, KZText.LoadText(pArg));
        return 1;
    }
    return 0;
}

static int KZTextBindClearCache(lua_State * ls)
{
    KZText.ClearCache();
    return 0;
}

static int KZTextBindAddCharacter(lua_State * ls)
{
    const char * pArg = luaL_checkstring(ls, 1);
    if(pArg)
    {
        lua_pushboolean(ls, KZText.AddCharacter((const unsigned char *)pArg));
        return 1;
    }
    return 0;
}

static int KZTextBindAddCharSet(lua_State * ls)
{
    const char * pArg = luaL_checkstring(ls, 1);
    if(pArg)
    {
        lua_pushboolean(ls, KZText.AddCharSet(pArg));
        return 1;
    }
    return 0;
}

static int KZTextBindSetCurrentColor(lua_State * ls)
{
    Uint8 r = luaL_checkinteger(ls, 1);
    Uint8 g = luaL_checkinteger(ls, 2);
    Uint8 b = luaL_checkinteger(ls, 3);
    Uint8 a = luaL_checkinteger(ls, 4);

    KZText.SetCurrentColor(r, g, b, a);
    return 0;
}

static int KZTextBindCalculateRenderSize(lua_State * ls)
{
    const char * pArg = luaL_checkstring(ls, 1);
    int x;
    int y;

    KZText.CalculateRenderSize(pArg, &x, &y);

    lua_pushinteger(ls, x);
    lua_pushinteger(ls, y);
    return 2;
}

static int KZTextBindRenderText(lua_State * ls)
{
    const char * pArg = luaL_checkstring(ls, 1);
    int x = (int)std::floor(luaL_checknumber(ls, 2));
    int y = (int)std::floor(luaL_checknumber(ls, 3));

    KZText.RenderText(pArg, x, y);
    return 0;
}

static int KZTextBindInitText(lua_State * ls)
{
    lua_pushboolean(ls, KZText.InitText());
    return 1;
}

static int KZTextBindSetOutline(lua_State * ls)
{
    bool r = lua_toboolean(ls, 1);
    KZText.SetOutline(r);
    return 0;
}

static int KZGlobalsBindSetGameName(lua_State * ls)
{
    const char * pArg = luaL_checkstring(ls, 1);
    KZGlobals.SetGameName(pArg);
    return 0;
}

static int KZGlobalsBindSetGameOrg(lua_State * ls)
{
    const char * pArg = luaL_checkstring(ls, 1);
    KZGlobals.SetGameOrg(pArg);
    return 0;
}

//KZWindow
static const luaL_Reg KZWindowBind[] = {
    {"ConvertToWindowDrawColor", KZWindowBindConverToWindowDrawColor},
    {"DrawFillRect", KZWindowBindDrawFillRect},
    {"DrawRect", KZWindowBindDrawRect},
    {"DrawLine", KZWindowBindDrawLine},
    {"DrawEllipse", KZWindowBindDrawEllipse},
    {"DrawFillEllipse", KZWindowBindDrawFillEllipse},
    {"Delay", KZWindowBindDelay},
    {"GetWidth", KZWindow_GetWidth},
    {"GetHeight", KZWindow_GetHeight},
    {"ChangeWindowTitle", KZWindowBindChangeWindowTitle},
    {"SetTargetDelay", KZWindow_SetTargetDelay},
    {NULL, NULL}
};

//KZImages
static const luaL_Reg KZImagesBind[] = {
    {"RenderImage", KZImagesBindRenderImage},
    {"RenderRotoZoom", KZImagesBindRenderRotoZoom},
    {"KeepAreaAsImage", KZImagesBindKeepAreaAsImage},
    {"IsImageSlotBusy", KZImagesBindIsImageSlotBusy},
    {"LoadImage", KZImagesBindLoadImage},
    {"UnloadImage", KZImagesBindUnloadImage},
    {"UnloadAllImages", KZImagesBindUnloadAllImages},
    {"GetImageWidth", KZWindow_GetImageWidth},
    {"GetImageHeight", KZWindow_GetImageHeight},
    {NULL, NULL}
};

//KZSounds
static const luaL_Reg KZSoundsBind[] = {
    {"LoadAudio", KZSoundsBindLoadAudio},
    {"LoadMusic", KZSoundsBindLoadMusic},
    {"LoadSoundEffect", KZSoundsBindLoadSoundEffect},
    {"IsSoundSlotBusy", KZSoundsBindIsSoundSlotBusy},
    {"PlayAudio", KZSoundsBindPlayAudio},
    {"StopAudio", KZSoundsBindStopAudio},
    {"UnloadAllSounds", KZSoundsBindUnloadAllSounds},
    {"UnloadSound", KZSoundsBindUnloadSound},
    {NULL, NULL}
};

//KZInput
static const luaL_Reg KZInputBind[] = {
    {"InputState", KZInput_GetInputState},
    {"EvilRandom", KZInputBindEvilRandom},
    {"SetTextInputMode", KZInputBindSetTextInputMode},
    {NULL, NULL}
};

//KZFiles
static const luaL_Reg KZFilesBind[] = {
    {"OpenFileForWriting", KZFilesBindOpenFileForWriting},
    {"OpenFileForReading", KZFilesBindOpenFileForReading},
    {"WriteLine", KZFilesBindWriteLine},
    {"GetNextLine", KZFilesBindGetNextLine},
    {"CloseFile", KZFilesBindCloseFile},
    {NULL, NULL}
};

//KZText
static const luaL_Reg KZTextBind[] = {
    {"LoadText", KZTextBindLoadText},
    {"ClearCache", KZTextBindClearCache},
    {"AddCharacter", KZTextBindAddCharacter},
    {"AddCharSet", KZTextBindAddCharSet},
    {"SetCurrentColor", KZTextBindSetCurrentColor},
    {"RenderText", KZTextBindRenderText},
    {"InitText", KZTextBindInitText},
    {"SetOutline", KZTextBindSetOutline},
    {"CalculateRenderSize", KZTextBindCalculateRenderSize},
    {NULL, NULL}
};

//KZGlobals
static const luaL_Reg KZGlobalsBind[] = {
    {"SetGameName", KZGlobalsBindSetGameName},
    {"SetGameOrg", KZGlobalsBindSetGameOrg},
    {NULL, NULL}
};

#ifdef __ANDROID__
    int KZ_Android_File_Loader(lua_State *ls)
    {
        const char * parg = luaL_checkstring(ls, 1);

        std::string actual_file_path = parg;

        //fix dots
        for (size_t i = 0; i < actual_file_path.size(); ++i)
        {
            if (actual_file_path[i] == '.')
                actual_file_path[i] = '/';
        }
        //fix extension
        if(actual_file_path.compare(actual_file_path.size() - 4, 4, ".lua") != 0)
        {
            actual_file_path += ".lua";
        }

        SDL_IOStream* pfile = SDL_IOFromFile(actual_file_path.c_str(), "rb");

        char filebytes[256] = {'\0'};
        std::string fullfilebytes;
        int bytesread = 0;
        do
        {
            bytesread = SDL_ReadIO(pfile, filebytes, sizeof(filebytes));
            fullfilebytes.append(filebytes, bytesread);
        } while(bytesread == sizeof(filebytes));

        int status = SDL_GetIOStatus(pfile);

        SDL_CloseIO(pfile);

        if(status == SDL_IO_STATUS_ERROR)
        {
            return 0;
        }

        int res = luaL_loadbuffer(ls, fullfilebytes.c_str(), fullfilebytes.size(), parg);

        return 1;
    }
#endif

bool CKZScripts::Init()
{
    MainLuaState = luaL_newstate();
    if(MainLuaState)
    {
        luaL_openlibs(MainLuaState);

        //push our own loader because android is weird
        #ifdef __ANDROID__
            lua_getglobal(MainLuaState, "package");
            lua_getfield(MainLuaState, -1, "loaders");

            int n = (int)lua_objlen(MainLuaState, -1);
            SDL_Log("number of loaders: %d", n);

            lua_pushcfunction(MainLuaState, KZ_Android_File_Loader);
            lua_rawseti(MainLuaState, -2, 2);

            lua_pop(MainLuaState, 2);
        #endif        


        //push needed functions
        luaL_register(MainLuaState, "KZWindow", KZWindowBind);
        luaL_register(MainLuaState, "KZImages", KZImagesBind);
        luaL_register(MainLuaState, "KZSounds", KZSoundsBind);
        luaL_register(MainLuaState, "KZInput", KZInputBind);
        luaL_register(MainLuaState, "KZFiles", KZFilesBind);
        luaL_register(MainLuaState, "KZText", KZTextBind);
        luaL_register(MainLuaState, "KZGlobals", KZGlobalsBind);
    }
    return MainLuaState != nullptr;
}

void CKZScripts::Close()
{
    if(MainLuaState)
    {
        lua_close(MainLuaState);
        MainLuaState = nullptr;
    }
}
