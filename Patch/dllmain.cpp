// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"

#include <AclAPI.h>
#include <fstream>

using namespace std;

#define DLLAPI extern "C" __declspec(dllexport)
#define ROUTINE DLLAPI DWORD WINAPI

typedef DWORD(WINAPI* Routine)(LPVOID);

#define __ROUTINE(X) ROUTINE X(LPVOID lParam)

HMODULE hModule = NULL;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        ::hModule = hModule;
        break;
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

void SetAcl(HANDLE hProcess)
{
    DWORD dwRet;
    EXPLICIT_ACCESSW ea[2];// Administrator
    PACL pNewDacl = NULL;

    ZeroMemory(&ea[0], 2 * sizeof(EXPLICIT_ACCESS));
    BuildExplicitAccessWithNameW(
        &ea[0],
        (LPWSTR)L"CURRENT_USER",
        GENERIC_WRITE | PROCESS_ALL_ACCESS | WRITE_DAC | DELETE | WRITE_OWNER | READ_CONTROL, 
        DENY_ACCESS,
        NO_INHERITANCE
    );
    BuildExplicitAccessWithNameW(
        &ea[1],
        (LPWSTR)L"EVERYONE",
        GENERIC_WRITE | PROCESS_ALL_ACCESS | WRITE_DAC | DELETE | WRITE_OWNER | READ_CONTROL,
        DENY_ACCESS,
        NO_INHERITANCE
    );

    dwRet = SetEntriesInAclW(2, ea, NULL, &pNewDacl);
    if (dwRet != 0)
    {
        std::ofstream o("E:\\C.txt", ios::out | ios::app);
        o << "o1 failed";
        o.close();
    }
    dwRet = SetSecurityInfo(hProcess, SE_KERNEL_OBJECT, DACL_SECURITY_INFORMATION, NULL, NULL, pNewDacl, NULL);
    if (dwRet != 0)
    {
        std::ofstream o("E:\\C.txt", ios::out|ios::app);
        o << "o2 failed";
        o.close();
    }

    std::ofstream o("E:\\C.txt", ios::out | ios::app);
    o << "success";
    o.close();
}

__ROUTINE(Start)
{
    //MessageBoxA(0, (char*)lParam, "", 0);
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    ZeroMemory(&pi, sizeof(pi));

    CreateProcessA(NULL, (LPSTR)lParam, NULL, NULL, FALSE, NULL, NULL, NULL, &si, &pi);
    //CloseHandle(CreateFileA("E:\\B.txt", GENERIC_ALL, NULL, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL));
    SetAcl(pi.hProcess);

    FreeLibraryAndExitThread(hModule, 0); 
    return 0;
}