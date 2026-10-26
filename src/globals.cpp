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

#include <cstring>
#include "globals.h"
#include "files.h"


CKZGlobals KZGlobals;

void CKZGlobals::SetGameName(const char * p_name)
{
    if(p_name && !Locked)
    {
        strlcpy(GameName, p_name, sizeof(GameName));
        KZFiles.ResetSavePath();
    }
}

void CKZGlobals::SetGameOrg(const char * p_name)
{
    if(p_name && !Locked)
    {
        strlcpy(GameOrg, p_name, sizeof(GameOrg));
        KZFiles.ResetSavePath();
    }
}
