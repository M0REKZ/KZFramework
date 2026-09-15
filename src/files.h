#ifndef PLUSKAIZO_FILES_H
#define PLUSKAIZO_FILES_H

#include <SDL3/SDL.h>
#include <fstream>

class CKZFiles
{
    char SavesPath[512] = {'\0'};
    std::string FileReadBuffer;

    std::fstream CurrentFile;

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
    bool OpenFileForReading(const char * ppath, bool bin);
    bool OpenFileForWriting(const char * ppath, bool bin);
    void CloseFile();
    //better not yet
    //bool DeleteFile(const char * ppath);
    const char * GetNextLine();
    bool WriteLine(std::string line);

    //not exposed to lua
    bool ResetSavePath();
};

extern CKZFiles KZFiles;

#endif
