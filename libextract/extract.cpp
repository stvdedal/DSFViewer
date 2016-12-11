#include "extract.h"

#define _CRT_SECURE_NO_DEPRECATE
#include <cstdio>
#include <cstdlib>

#ifdef _WIN32
#include <windows.h>
#endif

bool extract(const char* packed_file, const char* output_dir)
{
#ifdef _WIN32
    const char* bin = "\"C:\\Program Files\\7-Zip\\7z.exe\"";

    char cmd[512];
    snprintf(cmd, sizeof(cmd), "%s x \"%s\" -o%s", bin, packed_file, output_dir);

    STARTUPINFOA si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    bool result = false;

    if (CreateProcessA(NULL, cmd, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi))
    {
        WaitForSingleObject(pi.hProcess, INFINITE);
        
        DWORD exit_code;
        if (GetExitCodeProcess(pi.hProcess, &exit_code))
        {
            result = exit_code == EXIT_SUCCESS;
        }
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
    return result;
#else
#error NOT IMPLEMENTED
#endif
}
