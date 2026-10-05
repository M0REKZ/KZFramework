#ifndef PLUSKAIZO_SCRIPTS_H
#define PLUSKAIZO_SCRIPTS_H

extern "C"
{
    #include <lua.h>
    #include <lualib.h>
    #include <lauxlib.h>
}

#include <iostream>
#include <SDL3/SDL.h>

class CKZScripts
{
    int MainLuaFunc = LUA_NOREF;
public:
    static lua_State * MainLuaState;

    bool Init();
    void Close();

    //returns true on success, dont confuse with the lua functions.....
    inline bool RunScript(const char *pfilename)
    {
        if(!pfilename)
            return false;

        #ifdef __ANDROID__
            {
                SDL_IOStream* pfile = SDL_IOFromFile(pfilename, "rb");

                SDL_Log("HELOOOOO MAIN\n");

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
                    SDL_Log("ERROR READING FILE: %s\n", pfilename);
                    return false;
                }

                int res = luaL_loadbuffer(MainLuaState, fullfilebytes.c_str(), fullfilebytes.size(), pfilename);

                if(res)
                {
                    SDL_Log("ERROR LUAJIT: %s\n", lua_tostring(MainLuaState, -1));
                    lua_pop(MainLuaState, 1);
                    return false;
                }

                res = lua_pcall(MainLuaState, 0, LUA_MULTRET, 0);

                if(res)
                {
                    SDL_Log("ERROR LUAJIT: %s\n", lua_tostring(MainLuaState, -1));
                    lua_pop(MainLuaState, 1);
                    return false;
                }

                return true;
            }
        #else

            if(luaL_dofile(MainLuaState, pfilename))
            {
                SDL_Log("ERROR LUAJIT: %s\n", lua_tostring(MainLuaState, -1));
                lua_pop(MainLuaState, 1);
                return false;
            }

            return true;

        #endif
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

            if (lua_pcall(MainLuaState, 0, 0, 0) != 0 /* LUA_OK */)
            {
                SDL_Log("ERROR LUAJIT: %s\n", lua_tostring(MainLuaState, -1));
                lua_pop(MainLuaState, 1);
                return false;
            }
        }
        else
        {
            SDL_Log("ERROR LUAJIT: No Main Function\n");
            lua_pop(MainLuaState, 1);
            return false;
        }
        return true;
    }
};

extern CKZScripts KZScripts;

#endif
