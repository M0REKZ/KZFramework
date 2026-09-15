
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
