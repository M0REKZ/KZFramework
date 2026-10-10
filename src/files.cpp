/*
    KZFramework
    Copyright (C) Benjamín Gajardo

    This program is shared under the PLUSKAIZO LICENSE, you should have received
    a copy of the license along with the program.
*/

#include <SDL3/SDL.h>
#include "globals.h"
#include "files.h"
#include "crossplatform.h"

CKZFiles KZFiles;

bool CKZFiles::Init()
{
    SavesPath[0] = '\0';
    const char * pbase = SDL_GetBasePath();
    if(!pbase)
    {
        return false;
    }
    
    #ifdef KZFRAMEWORK_USE_GAME_DIR
        BasePath = std::string(pbase) + "game/";
    #else
        BasePath = std::string(pbase);
    #endif

    return true;
}

bool CKZFiles::OpenFileForReading(const char *ppath, bool bin, bool force_local)
{
    CloseFile();

    if(!SavesPath[0])
    {
        SDL_Log("Cant open file: Save path is not ready (set game name and game org to fix this)\n");
        return false;
    }

    // SavePath file can override local file
    if(!force_local)
    {
        std::string psavespath = std::string(SavesPath) + ppath;

        pCurrentFile = SDL_IOFromFile(psavespath.c_str(), bin ? "rb": "r");

        if(pCurrentFile)
        {
            CurrentFileState = bin ? EFileState::OPEN_READ_BIN : EFileState::OPEN_READ;
            return true;
        }
        else
        {
            CloseFile();
        }
    }

    std::string pcurrentpath = BasePath + ppath;

    pCurrentFile = SDL_IOFromFile(pcurrentpath.c_str(), bin ? "rb": "r");

    if(pCurrentFile)
    {
        CurrentFileState = bin ? EFileState::OPEN_READ_BIN : EFileState::OPEN_READ;
        return true;
    }
    else
    {
        CloseFile();
        return false;
    }
}

bool CKZFiles::OpenFileForWriting(const char *ppath, bool bin)
{
    CloseFile();

    if(!SavesPath[0])
    {
        SDL_Log("Cant open file: Save path is not ready (set game name and game org to fix this)\n");
        return false;
    }

    std::string psavespath = std::string(SavesPath) + ppath;

    pCurrentFile = SDL_IOFromFile(psavespath.c_str(), bin ? "wb": "w");

    if(pCurrentFile)
    {
        CurrentFileState = bin ? EFileState::OPEN_WRITE_BIN : EFileState::OPEN_WRITE;
        return true;
    }
    else
    {
        CloseFile();
        return false;
    }
}

void CKZFiles::CloseFile()
{
    if(pCurrentFile)
    {
        SDL_CloseIO(pCurrentFile);
        pCurrentFile = nullptr;
        KZCrossPlatform.FlushSavePath(); //mainly for emscripten
    }
    CurrentFileState = EFileState::CLOSED;
    return;
}

const char *CKZFiles::GetNextLine()
{
    if(CurrentFileState == EFileState::OPEN_READ && pCurrentFile)
    {
        if(SDL_GetIOStatus(pCurrentFile) == SDL_IO_STATUS_EOF)
        {
            return nullptr;
        }

        int bytesread = SDL_ReadIO(pCurrentFile, FileReadBuffer, sizeof(FileReadBuffer) - 1);
        FileReadBuffer[sizeof(FileReadBuffer) - 1] = '\0';

        if(!bytesread)
        {
            return nullptr;
        }

        return FileReadBuffer;
    }
    return nullptr;
}

bool CKZFiles::WriteLine(std::string line)
{
    if(!pCurrentFile)
    {
        SDL_Log("file is not open\n");
        return false;
    }

    if(SDL_GetIOStatus(pCurrentFile) != SDL_IO_STATUS_READY)
    {
        SDL_Log("file stream is broken\n");
        return false;
    }

    if(CurrentFileState == EFileState::OPEN_WRITE)
    {
        SDL_WriteIO(pCurrentFile, line.c_str(), line.size());
        return SDL_GetIOStatus(pCurrentFile) != SDL_IO_STATUS_ERROR;
    }
    SDL_Log("write failed\n");
    return false;
}

bool CKZFiles::ResetSavePath()
{
    if(!KZGlobals.GetGameName()[0] || !KZGlobals.GetGameOrg()[0])
    {
        return false;
    }

    char * ppath = SDL_GetPrefPath(KZGlobals.GetGameOrg(), KZGlobals.GetGameName());

    if(ppath)
    {
        strncpy(SavesPath, ppath, sizeof(SavesPath));
        SDL_free(ppath);
        return KZCrossPlatform.InitSavePath(SavesPath);
    }

    SDL_Log("ERROR INIT SDL SAVE PATH: %s\n", SDL_GetError());
    return false;
}
