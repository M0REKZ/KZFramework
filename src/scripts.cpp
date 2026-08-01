
extern "C"
{
    #include <lua.h>
    #include <lualib.h>
    #include <lauxlib.h>
}

#include "scripts.h"
#include "window.h"

lua_State * CKZScripts::MainLuaState = nullptr;

CKZScripts KZScripts;

static int KZWindowBindDrawFillRect(lua_State * ls)
{
    int x_from = luaL_checkinteger(ls, 1);
    int y_from = luaL_checkinteger(ls, 2);
    int x_to = luaL_checkinteger(ls, 3);
    int y_to = luaL_checkinteger(ls, 4);
    Uint32 color = luaL_checkinteger(ls, 5);

    KZWindow.DrawFillRect(x_from, y_from, x_to, y_to, color);
    return 1;
}

static int KZWindowBindConverToWindowDrawColor(lua_State * ls)
{
    Uint8 r = luaL_checkinteger(ls, 1);
    Uint8 g = luaL_checkinteger(ls, 2);
    Uint8 b = luaL_checkinteger(ls, 3);
    Uint8 a = luaL_checkinteger(ls, 4);

    lua_pushinteger(CKZScripts::MainLuaState, KZWindow.ConverToWindowDrawColor(r, g, b, a));
    return 1;
}

static const luaL_Reg KZWindowBind[] = {
    {"DrawFillRect", KZWindowBindDrawFillRect},
    {"ConvertToWindowDrawColor", KZWindowBindConverToWindowDrawColor},
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
