#ifndef PLUSKAIZO_GLOBALS_H
#define PLUSKAIZO_GLOBALS_H

class CKZGlobals
{
    char GameName[128] = {'\0'};
    char GameOrg[128] = {'\0'};
    unsigned char Locked = false;
public:
    void Lock() { Locked = true; }
    void SetGameName(const char * p_name);
    void SetGameOrg(const char * p_name);
    const char * GetGameName() { return GameName; }
    const char * GetGameOrg() { return GameOrg; }
};

extern CKZGlobals KZGlobals;

#endif
