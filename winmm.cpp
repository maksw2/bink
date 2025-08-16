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
#include <cstdint>

extern "C" {

typedef DWORD (*timeGetTime_t)();
timeGetTime_t ptimeGetTime = nullptr;

__declspec(dllexport) DWORD timeGetTime() {
    return ptimeGetTime();
}

HMODULE LoadLibraryExA(LPCSTR lpLibFileName, HANDLE hFile, DWORD dwFlags);

#define IMAGE_DOS_SIGNATURE 0x5A4D      // MZ
#define IMAGE_NT_SIGNATURE  0x00004550  // PE00
#define IMAGE_DIRECTORY_ENTRY_EXPORT 0

struct IMAGE_DOS_HEADER {
    uint16_t e_magic;
    uint16_t e_cblp;
    uint16_t e_cp;
    uint16_t e_crlc;
    uint16_t e_cparhdr;
    uint16_t e_minalloc;
    uint16_t e_maxalloc;
    uint16_t e_ss;
    uint16_t e_sp;
    uint16_t e_csum;
    uint16_t e_ip;
    uint16_t e_cs;
    uint16_t e_lfarlc;
    uint16_t e_ovno;
    uint16_t e_res[4];
    uint16_t e_oemid;
    uint16_t e_oeminfo;
    uint16_t e_res2[10];
    uint32_t e_lfanew;
};

struct IMAGE_FILE_HEADER {
    uint16_t Machine;
    uint16_t NumberOfSections;
    uint32_t TimeDateStamp;
    uint32_t PointerToSymbolTable;
    uint32_t NumberOfSymbols;
    uint16_t SizeOfOptionalHeader;
    uint16_t Characteristics;
};

struct IMAGE_DATA_DIRECTORY {
    uint32_t VirtualAddress;
    uint32_t Size;
};

struct IMAGE_OPTIONAL_HEADER64 {
    uint16_t Magic;
    uint8_t  MajorLinkerVersion;
    uint8_t  MinorLinkerVersion;
    uint32_t SizeOfCode;
    uint32_t SizeOfInitializedData;
    uint32_t SizeOfUninitializedData;
    uint32_t AddressOfEntryPoint;
    uint32_t BaseOfCode;
    uint64_t ImageBase;
    uint32_t SectionAlignment;
    uint32_t FileAlignment;
    uint16_t MajorOperatingSystemVersion;
    uint16_t MinorOperatingSystemVersion;
    uint16_t MajorImageVersion;
    uint16_t MinorImageVersion;
    uint16_t MajorSubsystemVersion;
    uint16_t MinorSubsystemVersion;
    uint32_t Win32VersionValue;
    uint32_t SizeOfImage;
    uint32_t SizeOfHeaders;
    uint32_t CheckSum;
    uint16_t Subsystem;
    uint16_t DllCharacteristics;
    uint64_t SizeOfStackReserve;
    uint64_t SizeOfStackCommit;
    uint64_t SizeOfHeapReserve;
    uint64_t SizeOfHeapCommit;
    uint32_t LoaderFlags;
    uint32_t NumberOfRvaAndSizes;
    IMAGE_DATA_DIRECTORY DataDirectory[16];
};

struct IMAGE_NT_HEADERS64 {
    uint32_t Signature;
    IMAGE_FILE_HEADER FileHeader;
    IMAGE_OPTIONAL_HEADER64 OptionalHeader;
};

struct IMAGE_EXPORT_DIRECTORY {
    uint32_t Characteristics;
    uint32_t TimeDateStamp;
    uint16_t MajorVersion;
    uint16_t MinorVersion;
    uint32_t Name;
    uint32_t Base;
    uint32_t NumberOfFunctions;
    uint32_t NumberOfNames;
    uint32_t AddressOfFunctions;     // RVA from base of image
    uint32_t AddressOfNames;         // RVA from base of image
    uint32_t AddressOfNameOrdinals;  // RVA from base of image
};

int strcmp(const char* a, const char* b) {
    while (*a && (*a == *b)) {
        ++a;
        ++b;
    }
    return static_cast<unsigned char>(*a) - static_cast<unsigned char>(*b);
}

FARPROC GetRealGetProcAddress(HMODULE hModule) {
    auto base = reinterpret_cast<uint8_t*>(hModule);
    auto dos = reinterpret_cast<IMAGE_DOS_HEADER*>(base);
    if (dos->e_magic != IMAGE_DOS_SIGNATURE) return nullptr;

    auto nt = reinterpret_cast<IMAGE_NT_HEADERS64*>(base + dos->e_lfanew);
    if (nt->Signature != IMAGE_NT_SIGNATURE) return nullptr;

    auto exportDirRVA = nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
    auto exportDir = reinterpret_cast<IMAGE_EXPORT_DIRECTORY*>(base + exportDirRVA);

    auto names = reinterpret_cast<uint32_t*>(base + exportDir->AddressOfNames);
    auto ordinals = reinterpret_cast<uint16_t*>(base + exportDir->AddressOfNameOrdinals);
    auto functions = reinterpret_cast<uint32_t*>(base + exportDir->AddressOfFunctions);

    for (uint32_t i = 0; i < exportDir->NumberOfNames; i++) {
        const char* name = reinterpret_cast<const char*>(base + names[i]);
        if (strcmp(name, "GetProcAddress") == 0) {
            uint16_t ord = ordinals[i];
            uint32_t funcRVA = functions[ord];
            return reinterpret_cast<FARPROC>(base + funcRVA);
        }
    }
    return nullptr;
}

typedef FARPROC (*GetProcAddress_t)(HMODULE, LPCSTR);
GetProcAddress_t pGetProcAddress = nullptr;

BOOL DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    if (fdwReason == 1) { // DLL_PROCESS_ATTACH
        HMODULE kernel32 = LoadLibraryExA("kernel32.dll", 0, 0x00000800);
        pGetProcAddress = (GetProcAddress_t)GetRealGetProcAddress(kernel32);

        ptimeGetTime = (timeGetTime_t)pGetProcAddress(kernel32, "timeGetTime");
    }
    return TRUE;
}

}
