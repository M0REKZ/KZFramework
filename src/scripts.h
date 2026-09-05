#ifndef PLUSKAIZO_SCRIPTS_H
#define PLUSKAIZO_SCRIPTS_H

extern "C"
{
    #include <lua.h>
    #include <lualib.h>
    #include <lauxlib.h>
}

#include <iostream>

class CKZScripts
{
    int MainLuaFunc = LUA_NOREF;
public:
    static lua_State * MainLuaState;

    bool Init();
    void Close();

    //returns true on success, dont confuse with the lua functions.....
    inline bool RunScript(const char *pfile)
    {
        if(!pfile)
            return false;

        if(luaL_dofile(MainLuaState, pfile))
        {
            printf("ERROR LUAJIT: %s\n", lua_tostring(MainLuaState, -1));
            lua_pop(MainLuaState, 1);
            return false;
        }

        return true;
    }

    inline bool RunMain()
    {
        if(MainLuaFunc == LUA_NOREF || MainLuaFunc == LUA_REFNIL)
        {
            lua_getglobal(MainLuaState, "KZMain");

            if(lua_isfunction(MainLuaState, -1))
            {
                MainLuaFunc = luaL_ref(MainLuaState, LUA_REGISTRYINDEX);
            }
        }

        if(MainLuaFunc != LUA_NOREF && MainLuaFunc != LUA_REFNIL)
        {
            lua_rawgeti(MainLuaState, LUA_REGISTRYINDEX, MainLuaFunc);

            if (lua_pcall(MainLuaState, 0, 0, 0) != LUA_OK)
            {
                std::cout << "ERROR LUAJIT: " << lua_tostring(MainLuaState, -1) << '\n';
                lua_pop(MainLuaState, 1);
                return false;
            }
        }
        else
        {
            std::cout << "ERROR LUAJIT: No Main Function" << std::endl;
            lua_pop(MainLuaState, 1);
            return false;
        }
        return true;
    }
};

extern CKZScripts KZScripts;

#endif
