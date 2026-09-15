#ifndef PLUSKAIZO_GLOBALS_H
#define PLUSKAIZO_GLOBALS_H

class CKZGlobals
{
    char GameName[128] = {'N','O','T','-','P','L','U','S','K','A','I','Z','O'};
    char GameOrg[128] = {'A','.','K','Z','F','r','a','m','e','w','o','r','k','.','g','a','m','e'};
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
