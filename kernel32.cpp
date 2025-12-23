#include <efi.h>
#include <efilib.h>
#include <intrin.h>
#include "print.h"

#pragma region types
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
typedef const void* LPCVOID;
typedef long LONG;
typedef long LONG_PTR;
typedef char* LPSTR;
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

typedef int BOOL;
typedef unsigned long DWORD;
typedef void* LPVOID;
typedef LPVOID LPCRITICAL_SECTION;

// Basic types
typedef void* PVOID;
typedef unsigned long DWORD;
typedef int BOOL;
typedef unsigned int UINT;
typedef unsigned short USHORT;
typedef unsigned long long ULONG64;
typedef unsigned long ULONG;
typedef void* HANDLE;
typedef long LONG;
typedef unsigned short WORD;
typedef unsigned char BYTE;
typedef long LONG_PTR;
typedef unsigned long long ULONG_PTR;
typedef unsigned char* LPBYTE;
typedef ULONG_PTR DWORD_PTR;

// Pointer to Const types
typedef const char* LPCSTR;
typedef const char* LPCTSTR;
typedef const void* LPCVOID;
typedef const wchar_t* LPCWSTR;

// Pointers to variables
typedef DWORD* LPDWORD;
typedef long* PLONG;
typedef char* LPSTR;
typedef char* LPCH;
typedef wchar_t* LPWSTR;
typedef wchar_t* LPWCH;
typedef BOOL* LPBOOL;
typedef void* LPVOID;
typedef void* HMODULE;

typedef long* LPLONG;
typedef ULONG64* PULONG64;

// Structures and callbacks
typedef struct _SECURITY_ATTRIBUTES {
    DWORD nLength;
    LPVOID lpSecurityDescriptor;
    BOOL bInheritHandle;
} SECURITY_ATTRIBUTES, *LPSECURITY_ATTRIBUTES;

typedef struct _OVERLAPPED {
    ULONG_PTR Internal;
    ULONG_PTR InternalHigh;
    DWORD Offset;
    DWORD OffsetHigh;
    HANDLE hEvent;
} OVERLAPPED, *LPOVERLAPPED;

typedef struct _STARTUPINFOA {
    DWORD cb;
    LPSTR lpReserved;
    LPSTR lpDesktop;
    LPSTR lpTitle;
    DWORD dwX;
    DWORD dwY;
    DWORD dwXSize;
    DWORD dwYSize;
    DWORD dwXCountChars;
    DWORD dwYCountChars;
    DWORD dwFillAttribute;
    DWORD dwFlags;
    USHORT wShowWindow;
    USHORT cbReserved2;
    LPBYTE lpReserved2;
    HANDLE hStdInput;
    HANDLE hStdOutput;
    HANDLE hStdError;
} STARTUPINFOA, *LPSTARTUPINFOA;

typedef struct _FILETIME {
    DWORD dwLowDateTime;
    DWORD dwHighDateTime;
} FILETIME, *LPFILETIME;

typedef struct _CPINFO {
    UINT MaxCharSize;
    BYTE DefaultChar[2];
    BYTE LeadByte[12];
} CPINFO, *LPCPINFO;

typedef unsigned short* LPWORD;
typedef char* PSTR;

// Function pointer for Fiber Local Storage callback
typedef void (*PFLS_CALLBACK_FUNCTION)(PVOID lpFlsData);

// Heap information enumeration
typedef enum _HEAP_INFORMATION_CLASS {
    HeapCompatibilityInformation,
    HeapEnableTerminationOnCorruption
} HEAP_INFORMATION_CLASS;

typedef struct _SYSTEM_INFO {
    union {
        DWORD dwOemId;
        struct {
            WORD wProcessorArchitecture;
            WORD wReserved;
        } DUMMYSTRUCTNAME;
    } DUMMYUNIONNAME;
    DWORD dwPageSize;
    LPVOID lpMinimumApplicationAddress;
    LPVOID lpMaximumApplicationAddress;
    DWORD_PTR dwActiveProcessorMask;
    DWORD dwNumberOfProcessors;
    DWORD dwProcessorType;
    DWORD dwAllocationGranularity;
    WORD wProcessorLevel;
    WORD wProcessorRevision;
} SYSTEM_INFO, *LPSYSTEM_INFO;

// Exception handling types
typedef PVOID PVECTORED_EXCEPTION_HANDLER;
typedef struct _CONTEXT {} CONTEXT, *PCONTEXT;
typedef struct _RUNTIME_FUNCTION {} RUNTIME_FUNCTION, *PRUNTIME_FUNCTION;
typedef LONG(*LPTOP_LEVEL_EXCEPTION_FILTER)(PVOID ExceptionInfo);

// Locale and Codepage types
typedef DWORD LCID;
typedef DWORD LCTYPE;
#pragma endregion

extern int stricmp(const char *s1, const char *s2);
extern void* Malloc(size_t size);
extern void Free(void* ptr);

// --- Memory ---
HANDLE HeapCreate(DWORD, SIZE_T, SIZE_T) {
    PRINT("Called: HeapCreate\n");
    return reinterpret_cast<HANDLE>(1);
}

BOOL HeapDestroy(HANDLE) {
    PRINT("Called: HeapDestroy\n");
    return TRUE; // nothing to do
}

LPVOID HeapAlloc(HANDLE, DWORD, SIZE_T size) {
    PRINT("Called: HeapAlloc with size: %ull\n", size);
    return Malloc(size);
}

BOOL HeapFree(HANDLE, DWORD, LPVOID ptr) {
    PRINT("Called: HeapFree with ptr: %p\n", ptr);
    if (ptr)
        Free(ptr);
    return TRUE;
}

LPVOID HeapReAlloc(HANDLE, DWORD, LPVOID ptr, SIZE_T size) {
    PRINT("Called: HeapReAlloc with size: %ull and ptr: %p\n", size, ptr);
    if (!ptr) return Malloc(size);
    LPVOID newPtr = Malloc(size);
    if (!newPtr) return nullptr;
    // No original size info; can't fully emulate, user must handle
    CopyMem(newPtr, ptr, size);
    Free(ptr);
    return newPtr;
}

SIZE_T HeapSize(HANDLE, DWORD, LPCVOID) {
    PRINT("Called: HeapSize\n");
    return 0;
}

// --- Threads (fake, UEFI is single-threaded at boot) ---
HANDLE GetCurrentThread() {
    PRINT("Called: GetCurrentThread\n");
    return reinterpret_cast<HANDLE>(1);
}

HANDLE CreateThread(void*, SIZE_T, LPTHREAD_START_ROUTINE, void*, DWORD, DWORD*) {
    PRINT("Called: CreateThread\n");
    return reinterpret_cast<HANDLE>(1);
}

DWORD WaitForSingleObject(HANDLE, DWORD) {
    PRINT("Called: WaitForSingleObject\n");
    return 0;
}

// --- Timing ---
static inline unsigned long long read_tsc() {
    _mm_lfence();
    return __rdtsc();
}

BOOL g_QpcInitialized = FALSE;
LARGE_INTEGER g_QpcFrequency;

static constexpr unsigned long long MS_PER_SECOND = 1000;

static unsigned long long g_TicksPerMs = 0;

BOOL QueryPerformanceFrequency(LARGE_INTEGER* lpFrequency)
{
    if (!lpFrequency)
        return FALSE;

    if (g_QpcInitialized)
    {
        *lpFrequency = g_QpcFrequency;
        return TRUE;
    }

    // 1. Calibrate for 1 full second to get accurate Ticks Per Second
    // gBS->Stall takes microseconds (1,000,000 us = 1 second)
    unsigned long long tscStart = read_tsc();
    gBS->Stall(1000000); 
    unsigned long long tscEnd = read_tsc();

    unsigned long long ticksPerSecond = tscEnd - tscStart;

    // 2. Set the "TicksPerMs" for our Counter to use later
    g_TicksPerMs = ticksPerSecond / MS_PER_SECOND;

    // 3. Set Frequency to 1000. 
    // This tells the engine: "One unit in QPC equals 1/1000th of a second (1ms)"
    g_QpcFrequency.QuadPart = MS_PER_SECOND;

    *lpFrequency = g_QpcFrequency;
    g_QpcInitialized = TRUE;
    return TRUE;
}

BOOL QueryPerformanceCounter(LARGE_INTEGER* lpPerformanceCount)
{
    if (!lpPerformanceCount || g_TicksPerMs == 0)
        return FALSE;

    // Now QPC returns the number of milliseconds that have passed.
    // Because Frequency is 1000, 1000 "counts" = 1 second. Perfectly synced.
    lpPerformanceCount->QuadPart = read_tsc() / g_TicksPerMs;
    
    return TRUE;
}

DWORD GetTickCount() {
    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);
    return (DWORD)(counter.QuadPart / 1000ULL); // ms
}

// --- Module / File ---
HMODULE GetModuleHandleA(LPCSTR name) {
    PRINT("Called: GetModuleHandleA with name: %s\n", name);
    return reinterpret_cast<HMODULE>(1);
}

void SleepConditionVariableCS(void*, void*, unsigned) {
    PRINT("Called: SleepConditionVariableCS\n");
}

BOOL TryEnterCriticalSection(LPCRITICAL_SECTION lpCriticalSection) {
    PRINT("Called: TryEnterCriticalSection\n");
    return TRUE;
}

BOOL GetLogicalProcessorInformation(void*, void*) {
    PRINT("Called: GetLogicalProcessorInformation\n");
    return FALSE;
}

void InitializeConditionVariable(void* cond) {
    PRINT("Called: InitializeConditionVariable\n");
    // No-op stub
}

int timeBeginPeriod(UINT period) {
    PRINT("Called: timeBeginPeriod\n");
    return 0;
}

int timeEndPeriod(UINT period) {
    PRINT("Called: timeEndPeriod\n");
    return 0;
}

void WakeConditionVariable(void* cond) {
    PRINT("Called: WakeConditionVariable\n");
    // No-op stub
}

void WakeAllConditionVariable(void* cond) {
    PRINT("Called: WakeAllConditionVariable\n");
    // No-op stub
}

BOOL InitializeCriticalSectionAndSpinCount(LPCRITICAL_SECTION lpCriticalSection, DWORD dwSpinCount) {
    PRINT("Called: InitializeCriticalSectionAndSpinCount\n");
    return TRUE;
}

int waveOutOpen() { PRINT("Called: waveOutOpen\n"); return 0; }
int waveOutPause() { PRINT("Called: waveOutPause\n"); return 0; }
int waveOutPrepareHeader() { PRINT("Called: waveOutPrepareHeader\n"); return 0; }
int waveOutUnprepareHeader() { PRINT("Called: waveOutUnprepareHeader\n"); return 0; }
int waveOutWrite() { PRINT("Called: waveOutWrite\n"); return 0; }
int waveOutReset() { PRINT("Called: waveOutReset\n"); return 0; }
int waveOutClose() { PRINT("Called: waveOutClose\n"); return 0; }
int waveOutRestart() { PRINT("Called: waveOutRestart\n"); return 0; }
int waveOutSetVolume() { PRINT("Called: waveOutSetVolume\n"); return 0; }

FARPROC GetProcAddress(HMODULE, LPCSTR func) {
    PRINT("Called: GetProcAddress with func: %s\n", func);
    if (stricmp(func, "waveOutOpen") == 0) {
        return reinterpret_cast<FARPROC>(waveOutOpen);
    }
    else if (stricmp(func, "waveOutPause") == 0) {
        return reinterpret_cast<FARPROC>(waveOutPause);
    }
    else if (stricmp(func, "waveOutPrepareHeader") == 0) {
        return reinterpret_cast<FARPROC>(waveOutPrepareHeader);
    }
    else if (stricmp(func, "waveOutUnprepareHeader") == 0) {
        return reinterpret_cast<FARPROC>(waveOutUnprepareHeader);
    }
    else if (stricmp(func, "waveOutWrite") == 0) {
        return reinterpret_cast<FARPROC>(waveOutWrite);
    }
    else if (stricmp(func, "waveOutReset") == 0) {
        return reinterpret_cast<FARPROC>(waveOutReset);
    }
    else if (stricmp(func, "waveOutClose") == 0) {
        return reinterpret_cast<FARPROC>(waveOutClose);
    }
    else if (stricmp(func, "waveOutRestart") == 0) {
        return reinterpret_cast<FARPROC>(waveOutRestart);
    }
    else if (stricmp(func, "waveOutSetVolume") == 0) {
        return reinterpret_cast<FARPROC>(waveOutSetVolume);
    }
    else if (stricmp(func, "SleepConditionVariableCS") == 0) {
        return reinterpret_cast<FARPROC>(SleepConditionVariableCS);
    }
    else if (stricmp(func, "TryEnterCriticalSection") == 0) {
        return reinterpret_cast<FARPROC>(TryEnterCriticalSection);
    }
    else if (stricmp(func, "GetLogicalProcessorInformation") == 0) {
        return reinterpret_cast<FARPROC>(GetLogicalProcessorInformation);
    }
    else if (stricmp(func, "InitializeConditionVariable") == 0) {
        return reinterpret_cast<FARPROC>(InitializeConditionVariable);
    }
    else if (stricmp(func, "timeBeginPeriod") == 0) {
        return reinterpret_cast<FARPROC>(timeBeginPeriod);
    }
    else if (stricmp(func, "timeEndPeriod") == 0) {
        return reinterpret_cast<FARPROC>(timeEndPeriod);
    }
    else if (stricmp(func, "WakeConditionVariable") == 0) {
        return reinterpret_cast<FARPROC>(WakeConditionVariable);
    }
    else if (stricmp(func, "InitializeCriticalSectionAndSpinCount") == 0) {
        return reinterpret_cast<FARPROC>(InitializeCriticalSectionAndSpinCount);
    }
    else if (stricmp(func, "WakeAllConditionVariable") == 0) {
        return reinterpret_cast<FARPROC>(WakeAllConditionVariable);
    }
    PRINT("Unknown function requested!\n");
    return nullptr;
}

HMODULE LoadLibraryA(LPCSTR name) {
    PRINT("Called: LoadLibraryA with name: %s\n", name);
    return reinterpret_cast<HMODULE>(1);
}

// --- Process ---
DWORD GetCurrentProcessId() {
    PRINT("Called: GetCurrentProcessId\n");
    return 1;
}

void ExitProcess(UINT uExitCode) {
    PRINT("Called: ExitProcess\n");
    while(1); // halt
}

DWORD GetSystemDirectoryA(LPSTR lpBuffer, DWORD uSize) {
    PRINT("Called: GetSystemDirectoryA\n");
    if (lpBuffer && uSize > 0) {
        *lpBuffer = '\0';
        return 1;
    }
    return 0;
}

DWORD GetWindowsDirectoryA(LPSTR lpBuffer, DWORD uSize) {
    PRINT("Called: GetWindowsDirectoryA\n");
    if (lpBuffer && uSize > 0) {
        *lpBuffer = '\0';
        return 1;
    }
    return 0;
}

DWORD GetModuleFileNameA(HMODULE hModule, LPSTR lpFilename, DWORD nSize) {
    PRINT("Called: GetModuleFileNameA\n");
    if (lpFilename && nSize > 0) {
        *lpFilename = '\0';
        return 1;
    }
    return 0;
}

void Sleep(DWORD dwMilliseconds) {
    PRINT("Called: Sleep\n");
    UINTN microseconds = (UINTN)dwMilliseconds * 1000;
    BS->Stall(microseconds);
}

DWORD SetErrorMode(DWORD uMode) {
    PRINT("Called: SetErrorMode\n");
    return 0;
}

DWORD GetEnvironmentVariableA(LPCTSTR lpName, LPSTR lpBuffer, DWORD nSize) {
    PRINT("Called: GetEnvironmentVariableA\n");
    if (lpBuffer && nSize > 0) lpBuffer[0] = 0;
    return 1;
}

BOOL CloseHandle(HANDLE hObject) {
    PRINT("Called: CloseHandle\n");
    return TRUE;
}

DWORD GetLastError() {
    PRINT("Called: GetLastError\n");
    return 0;
}

DWORD GetFileSize(HANDLE hFile, LPDWORD lpFileSizeHigh) {
    PRINT("Called: GetFileSize\n");
    if (lpFileSizeHigh) *lpFileSizeHigh = 0;
    return 0;
}

HANDLE CreateFileA(LPCTSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile) {
    PRINT("Called: CreateFileA\n");
    return NULL;
}

BOOL ReadFile(HANDLE hFile, LPVOID lpBuffer, DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead, LPOVERLAPPED lpOverlapped) {
    PRINT("Called: ReadFile\n");
    *lpNumberOfBytesRead = 0;
    return TRUE;
}

DWORD SetFilePointer(HANDLE hFile, LONG lDistanceToMove, PLONG lpDistanceToMoveHigh, DWORD dwMoveMethod) {
    PRINT("Called: SetFilePointer\n");
    return 0;
}

BOOL FreeLibrary(HMODULE hLibModule) {
    PRINT("Called: FreeLibrary\n");
    return TRUE;
}

int GetThreadPriority(HANDLE hThread) {
    PRINT("Called: GetThreadPriority\n");
    return 0;
}

PVOID RemoveVectoredExceptionHandler(PVOID Handle) {
    PRINT("Called: RemoveVectoredExceptionHandler\n");
    return NULL;
}

void RaiseException(DWORD dwExceptionCode, DWORD dwExceptionFlags, DWORD nNumberOfArguments, const ULONG_PTR* lpArguments) {
    PRINT("Called: RaiseException\n");
    return;
}

PVOID AddVectoredExceptionHandler(ULONG FirstHandler, PVECTORED_EXCEPTION_HANDLER VectoredHandler) {
    PRINT("Called: AddVectoredExceptionHandler\n");
    return NULL;
}

DWORD_PTR SetThreadAffinityMask(HANDLE hThread, DWORD_PTR dwThreadAffinityMask) {
    PRINT("Called: SetThreadAffinityMask\n");
    return 1;
}

DWORD ResumeThread(HANDLE hThread) {
    PRINT("Called: ResumeThread\n");
    return 1;
}

BOOL SetThreadPriority(HANDLE hThread, int nPriority) {
    PRINT("Called: SetThreadPriority\n");
    return TRUE;
}

void InitializeCriticalSection(LPCRITICAL_SECTION lpCriticalSection) {
    PRINT("Called: InitializeCriticalSection\n");
}

void DeleteCriticalSection(LPCRITICAL_SECTION lpCriticalSection) {
    PRINT("Called: DeleteCriticalSection\n");
}

void EnterCriticalSection(LPCRITICAL_SECTION lpCriticalSection) {
    PRINT("Called: EnterCriticalSection\n");
}

void LeaveCriticalSection(LPCRITICAL_SECTION lpCriticalSection) {
    PRINT("Called: LeaveCriticalSection\n");
}

DWORD GetCurrentThreadId() {
    PRINT("Called: GetCurrentThreadId\n");
    return 1;
}

HANDLE CreateMutexA(LPSECURITY_ATTRIBUTES lpMutexAttributes, BOOL bInitialOwner, LPCTSTR lpName) {
    PRINT("Called: CreateMutexA with name: lpName: %s\n", lpName);
    return reinterpret_cast<HANDLE>(1);
}

BOOL ReleaseMutex(HANDLE hMutex) {
    PRINT("Called: ReleaseMutex\n");
    return TRUE;
}

HANDLE CreateSemaphoreA(LPSECURITY_ATTRIBUTES lpSemaphoreAttributes, LONG lInitialCount, LONG lMaximumCount, LPCTSTR lpName) {
    PRINT("Called: CreateSemaphoreA with name: lpName: %s\n", lpName);
    return reinterpret_cast<HANDLE>(1);
}

BOOL ReleaseSemaphore(HANDLE hSemaphore, LONG lReleaseCount, LPLONG lpPreviousCount) {
    PRINT("Called: ReleaseSemaphore\n");
    if (lpPreviousCount) *lpPreviousCount = 0;
    return TRUE;
}

BOOL TerminateProcess(HANDLE hProcess, UINT uExitCode) {
    PRINT("Called: TerminateProcess\n");
    return TRUE;
}

HANDLE GetCurrentProcess() {
    PRINT("Called: GetCurrentProcess\n");
    return reinterpret_cast<HANDLE>(1);
}

LPTOP_LEVEL_EXCEPTION_FILTER UnhandledExceptionFilter(LPTOP_LEVEL_EXCEPTION_FILTER lpTopLevelExceptionFilter) {
    PRINT("Called: UnhandledExceptionFilter\n");
    return NULL;
}

LPTOP_LEVEL_EXCEPTION_FILTER SetUnhandledExceptionFilter(LPTOP_LEVEL_EXCEPTION_FILTER lpTopLevelExceptionFilter) {
    PRINT("Called: SetUnhandledExceptionFilter\n");
    return NULL;
}

BOOL IsDebuggerPresent() {
    PRINT("Called: IsDebuggerPresent\n");
    return FALSE;
}

PVOID RtlVirtualUnwind(ULONG HandlerType, ULONG64 ImageBase, ULONG64 ControlPc, PRUNTIME_FUNCTION FunctionEntry, PCONTEXT ContextRecord, PVOID* HandlerData, PULONG64 EstablisherFrame, PCONTEXT ContextRecord2) {
    PRINT("Called: RtlVirtualUnwind\n");
    return NULL;
}

PRUNTIME_FUNCTION RtlLookupFunctionEntry(ULONG64 ControlPc, PULONG64 ImageBase, PVOID HistoryTable) {
    PRINT("Called: RtlLookupFunctionEntry\n");
    return NULL;
}

void RtlCaptureContext(PCONTEXT ContextRecord) {
    PRINT("Called: RtlCaptureContext\n");
}

void RtlUnwindEx(PVOID TargetFrame, PVOID TargetIp, PVOID ExceptionRecord, PVOID ReturnValue, PCONTEXT ContextRecord, PVOID HistoryTable) {
    PRINT("Called: RtlUnwindEx\n");
}

BOOL WriteFile(HANDLE hFile, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite, LPDWORD lpNumberOfBytesWritten, LPOVERLAPPED lpOverlapped) {
    PRINT("Called: WriteFile\n");
    if (lpNumberOfBytesWritten) *lpNumberOfBytesWritten = 0;
    return TRUE;
}

HMODULE GetModuleHandleW(LPCWSTR lpModuleName) {
    PRINT("Called: GetModuleHandleW with name: %s\n", lpModuleName);
    return NULL;
}

int SetHandleCount(int n) {
    PRINT("Called: SetHandleCount\n");
    return 0;
}

HANDLE GetStdHandle(DWORD nStdHandle) {
    PRINT("Called: GetStdHandle\n");
    return reinterpret_cast<HANDLE>(1);
}

DWORD GetFileType(HANDLE hFile) {
    PRINT("Called: GetFileType\n");
    return 0;
}

void GetStartupInfoA(LPSTARTUPINFOA lpStartupInfo) {
    PRINT("Called: GetStartupInfoA\n");
}

LPCH GetEnvironmentStrings() {
    PRINT("Called: GetEnvironmentStrings\n");
    return NULL;
}

BOOL FreeEnvironmentStringsA(LPCH lpszString) {
    PRINT("Called: FreeEnvironmentStringsA\n");
    return TRUE;
}

LPWCH GetEnvironmentStringsW() {
    PRINT("Called: GetEnvironmentStringsW\n");
    return NULL;
}

BOOL FreeEnvironmentStringsW(LPWCH lpszString) {
    PRINT("Called: FreeEnvironmentStringsW\n");
    return TRUE;
}

void GetSystemTimeAsFileTime(LPFILETIME lpSystemTimeAsFileTime) {
    PRINT("Called: GetSystemTimeAsFileTime\n");
}

int GetACP() {
    PRINT("Called: GetACP\n");
    return 0;
}

BOOL GetCPInfo(UINT CodePage, LPCPINFO lpCPInfo) {
    PRINT("Called: GetCPInfo\n");
    return FALSE;
}

UINT GetOEMCP() {
    PRINT("Called: GetOEMCP\n");
    return 0;
}

BOOL IsValidCodePage(UINT CodePage) {
    PRINT("Called: IsValidCodePage\n");
    return FALSE;
}

int WideCharToMultiByte(UINT CodePage, DWORD dwFlags, LPCWSTR lpWideCharStr, int cchWideChar, LPSTR lpMultiByteStr, int cbMultiByte, LPCSTR lpDefaultChar, LPBOOL lpUsedDefaultChar) {
    PRINT("Called: WideCharToMultiByte\n");
    if (lpWideCharStr == NULL || cchWideChar <= 0) {
        return 0; // Return 0 for invalid input
    }
    
    int convertedCount = 0;
    while (*lpWideCharStr != '\0' && convertedCount < cchWideChar) {
        *lpMultiByteStr = (char)*lpWideCharStr;
        lpMultiByteStr++;
        lpWideCharStr++;
        convertedCount++;
    }
    
    // Null-terminate the string if there is space
    if (convertedCount < cchWideChar) {
        *lpMultiByteStr = '\0';
    }
    
    return convertedCount;
}

int LCMapStringA(LCID Locale, DWORD dwMapFlags, LPCSTR lpSrcStr, int cchSrc, LPSTR lpDestStr, int cchDest) {
    PRINT("Called: LCMapStringA\n");
    return 0;
}

int LCMapStringW(LCID Locale, DWORD dwMapFlags, LPCWSTR lpSrcStr, int cchSrc, LPWSTR lpDestStr, int cchDest) {
    PRINT("Called: LCMapStringW\n");
    return 0;
}

BOOL GetStringTypeA(LCID Locale, DWORD dwInfoType, LPCSTR lpSrcStr, int cchSrc, LPWORD lpCharType) {
    PRINT("Called: GetStringTypeA\n");
    return FALSE;
}

BOOL GetStringTypeW(DWORD dwInfoType, LPCWSTR lpSrcStr, int cchSrc, LPWORD lpCharType) {
    PRINT("Called: GetStringTypeW\n");
    return FALSE;
}

int GetLocaleInfoA(LCID Locale, LCTYPE LCType, LPSTR lpLCData, int cchData) {
    PRINT("Called: GetLocaleInfoA\n");
    *lpLCData = '\0';
    return 1;
}

BOOL FlsSetValue(DWORD dwFlsIndex, PVOID lpFlsData) {
    PRINT("Called: FlsSetValue\n");
    return TRUE;
}

PSTR GetCommandLineA() {
    PRINT("Called: GetCommandLineA\n");
    return NULL;
}

LPVOID EncodePointer(LPVOID Ptr) {
    PRINT("Called: EncodePointer\n");
    return Ptr;
}

LPVOID DecodePointer(LPVOID Ptr) {
    PRINT("Called: DecodePointer\n");
    return Ptr;
}

BOOL FlsGetValue(DWORD dwFlsIndex) {
    PRINT("Called: FlsGetValue\n");
    return NULL;
}

BOOL FlsFree(DWORD dwFlsIndex) {
    PRINT("Called: FlsFree\n");
    return TRUE;
}

void SetLastError(DWORD dwErrCode) {
    PRINT("Called: SetLastError\n");
}

DWORD FlsAlloc(PFLS_CALLBACK_FUNCTION lpCallback) {
    PRINT("Called: FlsAlloc\n");
    return 0;
}

BOOL HeapSetInformation(HANDLE HeapHandle, HEAP_INFORMATION_CLASS HeapInformationClass, PVOID HeapInformation, SIZE_T HeapInformationLength) {
    PRINT("Called: HeapSetInformation\n");
    return TRUE;
}

void GetSystemInfo(LPSYSTEM_INFO lpSystemInfo) {
    PRINT("Called: GetSystemInfo\n");
    if (lpSystemInfo) {
        // Fill the struct with dummy values consistent with an x64 environment
        lpSystemInfo->DUMMYUNIONNAME.DUMMYSTRUCTNAME.wProcessorArchitecture = 9; // PROCESSOR_ARCHITECTURE_AMD64
        lpSystemInfo->dwPageSize = 0x1000; // 4 KB page size
        lpSystemInfo->lpMinimumApplicationAddress = (LPVOID)0x10000;
        lpSystemInfo->lpMaximumApplicationAddress = (LPVOID)0x7FFFFFFFFFFFFFFF;
        lpSystemInfo->dwActiveProcessorMask = 1;
        lpSystemInfo->dwNumberOfProcessors = 1;
        lpSystemInfo->dwProcessorType = 8664; // PROCESSOR_AMD_X8664
        lpSystemInfo->dwAllocationGranularity = 0x10000; // 64 KB
        lpSystemInfo->wProcessorLevel = 6;
        lpSystemInfo->wProcessorRevision = 0;
    }
}

int MultiByteToWideChar(UINT CodePage, DWORD dwFlags, LPCSTR lpMultiByteStr, int cbMultiByte, LPWSTR lpWideCharStr, int cchWideChar) {
    PRINT("Called: MultiByteToWideChar\n");
    if (lpMultiByteStr == NULL || cchWideChar <= 0) {
        return 0; // Return 0 for invalid input
    }
    
    int convertedCount = 0;
    while (*lpMultiByteStr != '\0' && convertedCount < cchWideChar) {
        *lpWideCharStr = (wchar_t)*lpMultiByteStr;
        lpMultiByteStr++;
        lpWideCharStr++;
        convertedCount++;
    }
    
    // Null-terminate the string if there is space
    if (convertedCount < cchWideChar) {
        *lpWideCharStr = L'\0';
    }
    
    return convertedCount;
}

void* ResolveKernel32(const char* functionName) {
    if (stricmp(functionName, "HeapCreate") == 0) {
        return HeapCreate;
    } else if (stricmp(functionName, "HeapDestroy") == 0) {
        return HeapDestroy;
    } else if (stricmp(functionName, "HeapAlloc") == 0) {
        return HeapAlloc;
    } else if (stricmp(functionName, "HeapFree") == 0) {
        return HeapFree;
    } else if (stricmp(functionName, "HeapReAlloc") == 0) {
        return HeapReAlloc;
    } else if (stricmp(functionName, "HeapSize") == 0) {
        return HeapSize;
    } else if (stricmp(functionName, "GetCurrentThreadId") == 0) {
        return GetCurrentThreadId;
    } else if (stricmp(functionName, "GetCurrentThread") == 0) {
        return GetCurrentThread;
    } else if (stricmp(functionName, "CreateThread") == 0) {
        return CreateThread;
    } else if (stricmp(functionName, "WaitForSingleObject") == 0) {
        return WaitForSingleObject;
    } else if (stricmp(functionName, "CloseHandle") == 0) {
        return CloseHandle;
    } else if (stricmp(functionName, "QueryPerformanceCounter") == 0) {
        return QueryPerformanceCounter;
    } else if (stricmp(functionName, "QueryPerformanceFrequency") == 0) {
        return QueryPerformanceFrequency;
    } else if (stricmp(functionName, "GetTickCount") == 0) {
        return GetTickCount;
    } else if (stricmp(functionName, "GetModuleHandleA") == 0) {
        return GetModuleHandleA;
    } else if (stricmp(functionName, "GetProcAddress") == 0) {
        return GetProcAddress;
    } else if (stricmp(functionName, "LoadLibraryA") == 0) {
        return LoadLibraryA;
    } else if (stricmp(functionName, "GetCurrentProcessId") == 0) {
        return GetCurrentProcessId;
    } else if (stricmp(functionName, "GetCurrentProcess") == 0) {
        return GetCurrentProcess;
    } else if (stricmp(functionName, "ExitProcess") == 0) {
        return ExitProcess;
    } else if (stricmp(functionName, "InitializeCriticalSectionAndSpinCount") == 0) {
        return InitializeCriticalSectionAndSpinCount;
    } else if (stricmp(functionName, "GetSystemDirectoryA") == 0) {
        return GetSystemDirectoryA;
    } else if (stricmp(functionName, "GetWindowsDirectoryA") == 0) {
        return GetWindowsDirectoryA;
    } else if (stricmp(functionName, "GetModuleFileNameA") == 0) {
        return GetModuleFileNameA;
    } else if (stricmp(functionName, "Sleep") == 0) {
        return Sleep;
    } else if (stricmp(functionName, "SetErrorMode") == 0) {
        return SetErrorMode;
    } else if (stricmp(functionName, "GetEnvironmentVariableA") == 0) {
        return GetEnvironmentVariableA;
    } else if (stricmp(functionName, "CloseHandle") == 0) {
        return CloseHandle;
    } else if (stricmp(functionName, "GetLastError") == 0) {
        return GetLastError;
    } else if (stricmp(functionName, "GetFileSize") == 0) {
        return GetFileSize;
    } else if (stricmp(functionName, "CreateFileA") == 0) {
        return CreateFileA;
    } else if (stricmp(functionName, "ReadFile") == 0) {
        return ReadFile;
    } else if (stricmp(functionName, "SetFilePointer") == 0) {
        return SetFilePointer;
    } else if (stricmp(functionName, "FreeLibrary") == 0) {
        return FreeLibrary;
    } else if (stricmp(functionName, "GetThreadPriority") == 0) {
        return GetThreadPriority;
    } else if (stricmp(functionName, "RemoveVectoredExceptionHandler") == 0) {
        return RemoveVectoredExceptionHandler;
    } else if (stricmp(functionName, "RaiseException") == 0) {
        return RaiseException;
    } else if (stricmp(functionName, "AddVectoredExceptionHandler") == 0) {
        return AddVectoredExceptionHandler;
    } else if (stricmp(functionName, "SetThreadAffinityMask") == 0) {
        return SetThreadAffinityMask;
    } else if (stricmp(functionName, "ResumeThread") == 0) {
        return ResumeThread;
    } else if (stricmp(functionName, "SetThreadPriority") == 0) {
        return SetThreadPriority;
    } else if (stricmp(functionName, "InitializeCriticalSection") == 0) {
        return InitializeCriticalSection;
    } else if (stricmp(functionName, "DeleteCriticalSection") == 0) {
        return DeleteCriticalSection;
    } else if (stricmp(functionName, "EnterCriticalSection") == 0) {
        return EnterCriticalSection;
    } else if (stricmp(functionName, "LeaveCriticalSection") == 0) {
        return LeaveCriticalSection;
    } else if (stricmp(functionName, "GetCurrentThreadId") == 0) {
        return GetCurrentThreadId;
    } else if (stricmp(functionName, "CreateMutexA") == 0) {
        return CreateMutexA;
    } else if (stricmp(functionName, "ReleaseMutex") == 0) {
        return ReleaseMutex;
    } else if (stricmp(functionName, "CreateSemaphoreA") == 0) {
        return CreateSemaphoreA;
    } else if (stricmp(functionName, "ReleaseSemaphore") == 0) {
        return ReleaseSemaphore;
    } else if (stricmp(functionName, "TerminateProcess") == 0) {
        return TerminateProcess;
    } else if (stricmp(functionName, "GetCurrentProcess") == 0) {
        return GetCurrentProcess;
    } else if (stricmp(functionName, "UnhandledExceptionFilter") == 0) {
        return UnhandledExceptionFilter;
    } else if (stricmp(functionName, "SetUnhandledExceptionFilter") == 0) {
        return SetUnhandledExceptionFilter;
    } else if (stricmp(functionName, "IsDebuggerPresent") == 0) {
        return IsDebuggerPresent;
    } else if (stricmp(functionName, "RtlVirtualUnwind") == 0) {
        return RtlVirtualUnwind;
    } else if (stricmp(functionName, "RtlLookupFunctionEntry") == 0) {
        return RtlLookupFunctionEntry;
    } else if (stricmp(functionName, "RtlCaptureContext") == 0) {
        return RtlCaptureContext;
    } else if (stricmp(functionName, "RtlUnwindEx") == 0) {
        return RtlUnwindEx;
    } else if (stricmp(functionName, "WriteFile") == 0) {
        return WriteFile;
    } else if (stricmp(functionName, "GetModuleHandleW") == 0) {
        return GetModuleHandleW;
    } else if (stricmp(functionName, "SetHandleCount") == 0) {
        return SetHandleCount;
    } else if (stricmp(functionName, "GetStdHandle") == 0) {
        return GetStdHandle;
    } else if (stricmp(functionName, "GetFileType") == 0) {
        return GetFileType;
    } else if (stricmp(functionName, "GetStartupInfoA") == 0) {
        return GetStartupInfoA;
    } else if (stricmp(functionName, "GetEnvironmentStrings") == 0) {
        return GetEnvironmentStrings;
    } else if (stricmp(functionName, "FreeEnvironmentStringsA") == 0) {
        return FreeEnvironmentStringsA;
    } else if (stricmp(functionName, "GetEnvironmentStringsW") == 0) {
        return GetEnvironmentStringsW;
    } else if (stricmp(functionName, "FreeEnvironmentStringsW") == 0) {
        return FreeEnvironmentStringsW;
    } else if (stricmp(functionName, "GetSystemTimeAsFileTime") == 0) {
        return GetSystemTimeAsFileTime;
    } else if (stricmp(functionName, "GetACP") == 0) {
        return GetACP;
    } else if (stricmp(functionName, "GetCPInfo") == 0) {
        return GetCPInfo;
    } else if (stricmp(functionName, "GetOEMCP") == 0) {
        return GetOEMCP;
    } else if (stricmp(functionName, "IsValidCodePage") == 0) {
        return IsValidCodePage;
    } else if (stricmp(functionName, "WideCharToMultiByte") == 0) {
        return WideCharToMultiByte;
    } else if (stricmp(functionName, "LCMapStringA") == 0) {
        return LCMapStringA;
    } else if (stricmp(functionName, "LCMapStringW") == 0) {
        return LCMapStringW;
    } else if (stricmp(functionName, "GetStringTypeA") == 0) {
        return GetStringTypeA;
    } else if (stricmp(functionName, "GetStringTypeW") == 0) {
        return GetStringTypeW;
    } else if (stricmp(functionName, "GetLocaleInfoA") == 0) {
        return GetLocaleInfoA;
    } else if (stricmp(functionName, "FlsSetValue") == 0) {
        return FlsSetValue;
    } else if (stricmp(functionName, "GetCommandLineA") == 0) {
        return GetCommandLineA;
    } else if (stricmp(functionName, "EncodePointer") == 0) {
        return EncodePointer;
    } else if (stricmp(functionName, "DecodePointer") == 0) {
        return DecodePointer;
    } else if (stricmp(functionName, "FlsGetValue") == 0) {
        return FlsGetValue;
    } else if (stricmp(functionName, "FlsFree") == 0) {
        return FlsFree;
    } else if (stricmp(functionName, "SetLastError") == 0) {
        return SetLastError;
    } else if (stricmp(functionName, "FlsAlloc") == 0) {
        return FlsAlloc;
    } else if (stricmp(functionName, "GetSystemInfo") == 0) {
        return GetSystemInfo;
    } else if (stricmp(functionName, "HeapSetInformation") == 0) {
        return HeapSetInformation;
    } else if (stricmp(functionName, "MultiByteToWideChar") == 0) {
        return MultiByteToWideChar;
    }
    
    printf("Unknown function: %s\n", functionName);
    return NULL;
}
