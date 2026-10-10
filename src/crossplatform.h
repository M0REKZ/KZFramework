/*
    KZFramework
    Copyright (C) Benjamín Gajardo

    This program is shared under the PLUSKAIZO LICENSE, you should have received
    a copy of the license along with the program.
*/

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
