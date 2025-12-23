#include <efi.h>
#include <efilib.h>
#include "print.h"

typedef unsigned long DWORD;
typedef int BOOL;

typedef DWORD (*timeGetTime_t)();
timeGetTime_t ptimeGetTime = nullptr;

DWORD timeGetTime() {
    EFI_TIME t;
    if (EFI_SUCCESS == gRT->GetTime(&t, nullptr)) {
        // convert to milliseconds since midnight
        return (DWORD)(
            ((t.Hour * 3600 + t.Minute * 60 + t.Second) * 1000ULL) +
            (t.Nanosecond / 1000000ULL)
        );
    }
    return 0;
}

extern int stricmp(const char *s1, const char *s2);

void* ResolveWinMM(const char* functionName) {
    if (stricmp(functionName, "timeGetTime") == 0) {
        return timeGetTime;
    }
    
    printf("Unknown function: %s\n", functionName);
    return NULL;
}
