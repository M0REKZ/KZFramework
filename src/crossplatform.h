#ifndef KZFRAMEWORK_CROSSPLATFORM_H
#define KZFRAMEWORK_CROSSPLATFORM_H

// for platform specific things

class CKZCrossPlatform
{
    static void GameLoop();
public:
    bool InitSavePath(const char * path);
    bool FlushSavePath();
    void DoMainLoop();
};

extern CKZCrossPlatform KZCrossPlatform;

#endif
