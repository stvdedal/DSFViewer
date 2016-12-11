#include <XPLMUtilities.h>

#include "log.h"

#include <cstdio>
#include <stdarg.h>

static FILE* myLogFile = nullptr;

void openLog()
{
    myLogFile = fopen("olha.log", "w");
    if (!myLogFile) {
        XPLMDebugString("Olha: log file open error; aborting");
    }
}

void closeLog()
{
    fclose(myLogFile);
    myLogFile = nullptr;
}

void log(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    vfprintf(myLogFile, format, args);
    fflush(myLogFile);
    va_end(args);
}
