typedef void* HWND;
typedef void* LPUNKNOWN;
typedef void* REFIID;
typedef void* REFCLSID;
typedef long HRESULT;
typedef unsigned long DWORD;
typedef unsigned int UINT;
typedef int BOOL;
typedef void* HANDLE;
typedef void* LPVOID;
typedef long LONG;
typedef long LONG_PTR;
typedef const char* LPCSTR;
typedef size_t SIZE_T;
typedef void* HMODULE;
typedef HMODULE HINSTANCE;
typedef void* FARPROC;
typedef DWORD (*LPTHREAD_START_ROUTINE)(LPVOID lpThreadParameter);
typedef union _LARGE_INTEGER {
    struct {
        unsigned long LowPart;
        long HighPart;
    };
    long long QuadPart;
} LARGE_INTEGER;
#define S_OK ((HRESULT)0L)
#define TRUE 1
#define FALSE 0

extern "C" {

// COM stubs
__declspec(dllexport) HRESULT CoCreateInstance(REFCLSID, LPUNKNOWN, DWORD, REFIID, LPVOID* ppv) {
    if (ppv) *ppv = nullptr;
    return S_OK;
}
__declspec(dllexport) HRESULT CoInitializeEx(LPVOID, DWORD) { return S_OK; }
__declspec(dllexport) void CoUninitialize() {}
__declspec(dllexport) void CoTaskMemFree(LPVOID) {}
__declspec(dllexport) HRESULT PropVariantClear(LPVOID) { return S_OK; }

BOOL DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) { return TRUE; }

}
