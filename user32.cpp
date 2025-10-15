#include <efi.h>
#include <efilib.h>
#include "print.h"

typedef void* HWND;
typedef unsigned long DWORD;
typedef unsigned int UINT;
typedef long LONG;
typedef long LONG_PTR;
typedef const char* LPCSTR;

// Window stubs
HWND GetDesktopWindow() { PRINT(L"GetDesktopWindow called\n"); return (HWND)1; }
HWND GetActiveWindow() { PRINT(L"GetActiveWindow called\n"); return (HWND)1; }
HWND GetTopWindow(HWND) { PRINT(L"GetTopWindow called\n"); return (HWND)1; }
HWND GetWindow(HWND, UINT) { PRINT(L"GetWindow called\n"); return (HWND)1; }
LONG GetWindowLongA(HWND, int) { PRINT(L"GetWindowLongA called\n"); return 0; }
LONG_PTR GetWindowLongPtrA(HWND, int) { PRINT(L"GetWindowLongPtrA called\n"); return 0; }
void GetWindowThreadProcessId(HWND, DWORD*) { PRINT(L"GetWindowThreadProcessId called\n"); }

// MessageBox
int MessageBoxA(HWND, LPCSTR text, LPCSTR caption, UINT) {
    PRINT(L"MessageBoxA called\n");
    PRINT(L"[MessageBox] %s: %s\n", caption, text);
    return 1;
}

extern int stricmp(const char *s1, const char *s2);

void* ResolveUser32(const char* functionName) {
    if (stricmp(functionName, "GetDesktopWindow") == 0) {
        return GetDesktopWindow;
    } else if (stricmp(functionName, "GetActiveWindow") == 0) {
        return GetActiveWindow;
    } else if (stricmp(functionName, "GetTopWindow") == 0) {
        return GetTopWindow;
    } else if (stricmp(functionName, "GetWindow") == 0) {
        return GetWindow;
    } else if (stricmp(functionName, "GetWindowLongA") == 0) {
        return GetWindowLongA;
    } else if (stricmp(functionName, "GetWindowLongPtrA") == 0) {
        return GetWindowLongPtrA;
    } else if (stricmp(functionName, "GetWindowThreadProcessId") == 0) {
        return GetWindowThreadProcessId;
    } else if (stricmp(functionName, "MessageBoxA") == 0) {
        return MessageBoxA;
    }

    Print(L"Unknown function: %a\n", functionName);
    return NULL;
}
