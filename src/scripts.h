#ifndef PLUSKAIZO_SCRIPTS_H
#define PLUSKAIZO_SCRIPTS_H

extern "C"
{
    #include <lua.h>
    #include <lualib.h>
    #include <lauxlib.h>
}

class CKZScripts
{
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
};

extern CKZScripts KZScripts;

#endif
