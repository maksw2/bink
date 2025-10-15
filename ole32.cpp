#include <efi.h>
#include <efilib.h>
#include "print.h"

typedef void* LPUNKNOWN;
typedef void* REFIID;
typedef void* REFCLSID;
typedef long HRESULT;
typedef unsigned long DWORD;
typedef int BOOL;
typedef void* LPVOID;
typedef void* HMODULE;
#define S_OK ((HRESULT)0L)

// COM stubs
HRESULT CoCreateInstance(REFCLSID, LPUNKNOWN, DWORD, REFIID, LPVOID* ppv) {
    PRINT(L"CoCreateInstance called\n");
    if (ppv) *ppv = nullptr;
    return S_OK;
}
HRESULT CoInitializeEx(LPVOID, DWORD) { PRINT(L"CoInitializeEx called\n"); return S_OK; }
void CoUninitialize() { PRINT(L"CoUninitialize called\n"); }
void CoTaskMemFree(LPVOID) { PRINT(L"CoTaskMemFree called\n"); }
HRESULT PropVariantClear(LPVOID) { PRINT(L"PropVariantClear called\n"); return S_OK; }

extern int stricmp(const char *s1, const char *s2);

void* ResolveOle32(const char* functionName) {
    if (stricmp(functionName, "CoCreateInstance") == 0) {
        return CoCreateInstance;
    } else if (stricmp(functionName, "CoInitializeEx") == 0) {
        return CoInitializeEx;
    } else if (stricmp(functionName, "CoUninitialize") == 0) {
        return CoUninitialize;
    } else if (stricmp(functionName, "CoTaskMemFree") == 0) {
        return CoTaskMemFree;
    } else if (stricmp(functionName, "PropVariantClear") == 0) {
        return PropVariantClear;
    }
    
    Print(L"Unknown function: %a\n", functionName);
    return NULL;
}
