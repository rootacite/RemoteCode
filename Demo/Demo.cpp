// Demo.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <AclAPI.h>
#include "RemoteCode.hpp"
#include <winerror.h>

int main()
{
    char path[] = "CrackMe.exe";

 
    int PID = 1064;
    unsigned int Code = 0;

    HMODULE hPatch = LoadLibraryA("Patch.dll");

    RemoteCode rc(PID, hPatch);
    auto c = rc.write(path);
    Code = rc.invoke("Start", c);
    rc.release(c);
    std::cout << Code;
    getchar();
}
