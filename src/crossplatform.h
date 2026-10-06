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
    void UpdateTouchControls();
    void HandleTouchInput();
    void DrawBasicTouchButtons();
};

extern CKZCrossPlatform KZCrossPlatform;

#endif
