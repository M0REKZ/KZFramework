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
