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

#ifndef PLUSKAIZO_FILES_H
#define PLUSKAIZO_FILES_H

#include <SDL3/SDL.h>
#include <fstream>

class CKZFiles
{
    char SavesPath[512] = {'\0'};
    std::string BasePath;
    char FileReadBuffer[1024] = {'\0'};

    //using SDL for now because fstream in android is weird
    SDL_IOStream * pCurrentFile = nullptr;

    enum class EFileState : Uint8
    {
        CLOSED = 0,
        OPEN_READ,
        OPEN_READ_BIN,
        OPEN_WRITE,
        OPEN_WRITE_BIN,
    };

    EFileState CurrentFileState = EFileState::CLOSED;
public:
    bool Init();
    // force_local will force check for files in game directory only
    bool OpenFileForReading(const char * ppath, bool bin, bool force_local);
    bool OpenFileForWriting(const char * ppath, bool bin);
    void CloseFile();
    //better not yet
    //bool DeleteFile(const char * ppath);
    const char * GetNextLine();
    bool WriteLine(std::string line);

    //not exposed to lua
    bool ResetSavePath();
    const std::string GetBasePath() { return BasePath; };
};

extern CKZFiles KZFiles;

#endif
