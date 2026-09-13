
#include <SDL3/SDL.h>
#include "files.h"

CKZFiles KZFiles;

bool CKZFiles::Init()
{
    char * ppath = SDL_GetPrefPath("m0rekz.github.io","PLUSKAIZO");

    if(ppath)
    {
        strncpy(SavesPath, ppath, sizeof(SavesPath));
        SDL_free(ppath);
        return true;
    }

    printf("ERROR INIT SDL SAVE PATH: %s\n", SDL_GetError());
    return false;
}

bool CKZFiles::OpenFileForReading(const char *ppath, bool bin)
{
    CloseFile();

    std::string psavespath = std::string(SavesPath) + ppath;

    CurrentFile.open(psavespath, std::ios::in | (bin ? std::ios::binary : 0));

    if(CurrentFile.is_open())
    {
        if(CurrentFile.bad())
        {
            CloseFile();
            return false;
        }
        CurrentFileState = bin ? EFileState::OPEN_READ_BIN : EFileState::OPEN_READ;
        return true;
    }

    return false;
}

bool CKZFiles::OpenFileForWriting(const char *ppath, bool bin)
{
    CloseFile();

    std::string psavespath = std::string(SavesPath) + ppath;

    CurrentFile.open(psavespath, std::ios::out | (bin ? std::ios::binary : 0));

    if(CurrentFile.is_open())
    {
        if(CurrentFile.bad())
        {
            CloseFile();
            return false;
        }
        CurrentFileState = bin ? EFileState::OPEN_WRITE_BIN : EFileState::OPEN_WRITE;
        return true;
    }

    return false;
}

void CKZFiles::CloseFile()
{
    if(CurrentFile.is_open())
    {
        CurrentFile.close();
    }
    CurrentFileState = EFileState::CLOSED;
    return;
}

const char *CKZFiles::GetNextLine()
{
    if(CurrentFileState == EFileState::OPEN_READ && CurrentFile.good())
    {
        if(CurrentFile.eof())
        {
            return nullptr;
        }

        std::getline(CurrentFile, FileReadBuffer);

        if(CurrentFile.rdstate() & std::ios::badbit)
        {
            return nullptr;
        }

        if(!CurrentFile.eof() && CurrentFile.rdstate() & std::ios::failbit)
        {
            CurrentFile.clear();
        }
        return FileReadBuffer.c_str();
    }
    return nullptr;
}

bool CKZFiles::WriteLine(std::string line)
{
    if(!CurrentFile.is_open())
    {
        printf("file is not open\n");
        return false;
    }

    if(!CurrentFile.good())
    {
        printf("file stream is broken\n");
        return false;
    }

    if(CurrentFileState == EFileState::OPEN_WRITE)
    {
        CurrentFile << line << '\n';
        return !CurrentFile.fail();
    }
    printf("write failed\n");
    return false;
}
