
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
    lua_newtable(ls);

    lua_pushboolean(ls, KZInput.InputState.Up);
    lua_setfield(ls, -2, "Up");

    lua_pushboolean(ls, KZInput.InputState.Down);
    lua_setfield(ls, -2, "Down");

    lua_pushboolean(ls, KZInput.InputState.Left);
    lua_setfield(ls, -2, "Left");

    lua_pushboolean(ls, KZInput.InputState.Right);
    lua_setfield(ls, -2, "Right");

    lua_pushboolean(ls, KZInput.InputState.Jump);
    lua_setfield(ls, -2, "Jump");

    lua_pushboolean(ls, KZInput.InputState.AltJump);
    lua_setfield(ls, -2, "AltJump");

    lua_pushboolean(ls, KZInput.InputState.Run);
    lua_setfield(ls, -2, "Run");

    lua_pushboolean(ls, KZInput.InputState.AltRun);
    lua_setfield(ls, -2, "AltRun");

    lua_pushnumber(ls, KZInput.InputState.MouseX);
    lua_setfield(ls, -2, "MouseX");

    lua_pushnumber(ls, KZInput.InputState.MouseY);
    lua_setfield(ls, -2, "MouseY");

    lua_pushboolean(ls, KZInput.InputState.MouseClick);
    lua_setfield(ls, -2, "MouseClick");

    lua_pushboolean(ls, KZInput.InputState.MouseRightClick);
    lua_setfield(ls, -2, "MouseRightClick");

    return 1;
}

static int KZInputBindEvilRandom(lua_State * ls)
{
    lua_pushinteger(ls, KZInput.EvilRandom());
    return 1;
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
    if(pArg)
    {
        lua_pushboolean(ls, KZFiles.OpenFileForReading(pArg, bin));
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
    {"UnloadAllSounds", KZSoundsBindUnloadAllSounds},
    {"UnloadSound", KZSoundsBindUnloadSound},
    {NULL, NULL}
};

//KZInput
static const luaL_Reg KZInputBind[] = {
    {"InputState", KZInput_GetInputState},
    {"EvilRandom", KZInputBindEvilRandom},
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


bool CKZScripts::Init()
{
    MainLuaState = luaL_newstate();
    if(MainLuaState)
    {
        luaL_openlibs(MainLuaState);

        //push needed functions
        luaL_register(MainLuaState, "KZWindow", KZWindowBind);
        luaL_register(MainLuaState, "KZImages", KZImagesBind);
        luaL_register(MainLuaState, "KZSounds", KZSoundsBind);
        luaL_register(MainLuaState, "KZInput", KZInputBind);
        luaL_register(MainLuaState, "KZFiles", KZFilesBind);
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
