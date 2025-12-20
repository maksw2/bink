#include <efi.h>
#include <efilib.h>
#include <x86_64\pe.h>
#include "bink.h"
#include "print.h"

struct BlockHeader {
    uint32_t magic;
    size_t size;
    bool free;
    BlockHeader* next;
};

constexpr uint32_t MAGIC_ALLOCATED = 0xCAFEBABE;
constexpr uint32_t MAGIC_FREED = 0xDEADBEEF;

static void* g_memoryPool = 0;
static size_t g_poolSize = 0;
static struct BlockHeader* g_freeList = 0;

#define AVX_ALIGN 32
#define ALIGN_UP(x, a) (((x) + ((a) - 1)) & ~((a) - 1))

void InitAllocatorFromBuffer(void* buffer, size_t size) {
    PRINT(L"Initializing custom allocator with buffer %p of size %ull\n", buffer, size);
    if (!buffer || size <= sizeof(struct BlockHeader)) {
        Print(L"Invalid buffer or size for allocator initialization\n");
        gBS->Stall(5 * 1000000); // 5 seconds
        gRT->ResetSystem(EfiResetShutdown, EFI_SUCCESS, 0, NULL);
    }

    g_memoryPool = buffer;
    g_poolSize = size;

    g_freeList = (struct BlockHeader*)buffer;
    g_freeList->magic = MAGIC_ALLOCATED;
    g_freeList->size = size - sizeof(struct BlockHeader);
    g_freeList->free = true;
    g_freeList->next = 0;
}

static bool IsPointerInPool(void* ptr) {
    return ptr >= g_memoryPool &&
        ptr < (void*)((uint8_t*)g_memoryPool + g_poolSize);
}

void* Malloc(size_t size) {
    PRINT(L"Allocating memory of size %llu, ptr = ", size);
    size = ALIGN_UP(size, AVX_ALIGN);  // ensure AVX alignment
    struct BlockHeader* best = 0;
    struct BlockHeader* bestPrev = 0;
    struct BlockHeader* prev = 0;
    struct BlockHeader* current = g_freeList;

    while (current) {
        if (current->free && current->size >= size) {
            if (!best || current->size < best->size) {
                best = current;
                bestPrev = prev;
            }
        }
        prev = current;
        current = current->next;
    }

    if (!best) {
        PRINT(L"NULL\n");
        return 0;
    }

    uintptr_t payloadAddr = (uintptr_t)(best + 1);
    uintptr_t alignedAddr = ALIGN_UP(payloadAddr, AVX_ALIGN);
    size_t padding = alignedAddr - payloadAddr;

    size_t totalNeeded = size + padding;
    if (best->size < totalNeeded) {
        PRINT(L"NULL not enough space\n");
        return 0; // not enough space
    }

    size_t remaining = best->size - totalNeeded;

    if (remaining > sizeof(struct BlockHeader)) {
        struct BlockHeader* newBlock = (struct BlockHeader*)((uint8_t*)(best + 1) + totalNeeded);
        newBlock->magic = MAGIC_ALLOCATED;
        newBlock->size = remaining - sizeof(struct BlockHeader);
        newBlock->free = true;
        newBlock->next = best->next;
        best->next = newBlock;
    }

    best->magic = MAGIC_ALLOCATED;
    best->free = false;
    best->size = totalNeeded;

    PRINT(L"%p\n", (void*)alignedAddr);
    return (void*)alignedAddr;
}

void Free(void* ptr) {
    PRINT(L"Freeing memory at %p\n", ptr);
    if (!ptr || !IsPointerInPool(ptr)) return;

    struct BlockHeader* block = (struct BlockHeader*)((uint8_t*)ptr - sizeof(struct BlockHeader));

    while (block->magic != MAGIC_ALLOCATED && (uint8_t*)block > (uint8_t*)g_memoryPool) {
        block = (struct BlockHeader*)((uint8_t*)block - 1);  // backtrack if pointer was aligned forward
    }

    if (!IsPointerInPool(block) || block->magic != MAGIC_ALLOCATED) {
        Print(L"Invalid pointer passed to Free: %p\n", ptr);
        return;
    }

    block->magic = MAGIC_FREED;
    block->free = true;

    // Coalesce
    struct BlockHeader* current = g_freeList;
    while (current) {
        if (current->free) {
            struct BlockHeader* next = current->next;
            while (next && next->free) {
                current->size += sizeof(struct BlockHeader) + next->size;
                current->next = next->next;
                next = next->next;
            }
        }
        current = current->next;
    }
}

#define IMAGE_NT_OPTIONAL_HDR64_MAGIC 0x20b
#define IMAGE_DIRECTORY_ENTRY_EXPORT 0
#define IMAGE_DIRECTORY_ENTRY_IMPORT 1
#define IMAGE_DIRECTORY_ENTRY_BASERELOC 5
#define IMAGE_ORDINAL_FLAG64 0x8000000000000000ULL

#pragma pack(push, 1)
typedef struct IMAGE_OPTIONAL_HEADER64 {
    uint16_t Magic;
    uint8_t MajorLinkerVersion;
    uint8_t MinorLinkerVersion;
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
    IMAGE_DATA_DIRECTORY DataDirectory[IMAGE_NUMBEROF_DIRECTORY_ENTRIES];
} __attribute__((packed)) IMAGE_OPTIONAL_HEADER64;

typedef struct IMAGE_NT_HEADERS64 {
    uint32_t Signature;
    struct {
        uint16_t Machine;
        uint16_t NumberOfSections;
        uint32_t TimeDateStamp;
        uint32_t PointerToSymbolTable;
        uint32_t NumberOfSymbols;
        uint16_t SizeOfOptionalHeader;
        uint16_t Characteristics;
    } FileHeader;
    IMAGE_OPTIONAL_HEADER64 OptionalHeader;
} __attribute__((packed)) IMAGE_NT_HEADERS64;

typedef struct _IMAGE_IMPORT_BY_NAME {
    uint16_t Hint;
    char Name[1];
} __attribute__((packed)) IMAGE_IMPORT_BY_NAME, *PIMAGE_IMPORT_BY_NAME;

typedef struct _IMAGE_THUNK_DATA64 {
    union {
        uint64_t ForwarderString;
        uint64_t Function;
        uint64_t Ordinal;
        uint64_t AddressOfData;
    } u1;
} __attribute__((packed)) IMAGE_THUNK_DATA64, *PIMAGE_THUNK_DATA64;

typedef struct _IMAGE_IMPORT_DESCRIPTOR {
    union {
        uint32_t Characteristics;
        uint32_t OriginalFirstThunk;
    } u1;
    uint32_t TimeDateStamp;
    uint32_t ForwarderChain;
    uint32_t Name;
    uint32_t FirstThunk;
} __attribute__((packed)) IMAGE_IMPORT_DESCRIPTOR, *PIMAGE_IMPORT_DESCRIPTOR;

typedef struct _IMAGE_EXPORT_DIRECTORY {
    uint32_t Characteristics;
    uint32_t TimeDateStamp;
    uint16_t MajorVersion;
    uint16_t MinorVersion;
    uint32_t Name;
    uint32_t Base;
    uint32_t NumberOfFunctions;
    uint32_t NumberOfNames;
    uint32_t AddressOfFunctions;
    uint32_t AddressOfNames;
    uint32_t AddressOfNameOrdinals;
} __attribute__((packed)) IMAGE_EXPORT_DIRECTORY, *PIMAGE_EXPORT_DIRECTORY;
#pragma pack(pop)

int stricmp(const char *s1, const char *s2) { while (*s1 && ((*s1 | 0x20) == (*s2 | 0x20))) { s1++; s2++; } return *(const unsigned char*)s1 - *(const unsigned char*)s2; }

extern void* ResolveUser32(const char* functionName);
extern void* ResolveKernel32(const char* functionName);
extern void* ResolveOle32(const char* functionName);
extern void* ResolveWinMM(const char* functionName);

void* ResolveImport(const char* dllName, const char* functionName) {
    // user32
    if (stricmp(dllName, "user32.dll") == 0) {
        return ResolveUser32(functionName);
    } else if (stricmp(dllName, "kernel32.dll") == 0) {
        return ResolveKernel32(functionName);
    } else if (stricmp(dllName, "ole32.dll") == 0) {
        return ResolveOle32(functionName);
    } else if (stricmp(dllName, "winmm.dll") == 0) {
        return ResolveWinMM(functionName);
    }

    // If the import is not a known dependency, print an error
    Print(L"Unknown dll: %a\n", dllName);
    return NULL;
}

void* GetExportByName(void* imageBase, const char* functionName) {
    if (!imageBase || !functionName) return NULL;

    uint8_t* base = (uint8_t*)imageBase;
    IMAGE_DOS_HEADER* dos = (IMAGE_DOS_HEADER*)base;
    IMAGE_NT_HEADERS64* pe = (IMAGE_NT_HEADERS64*)(base + dos->e_lfanew);

    // Get the export directory
    IMAGE_DATA_DIRECTORY exportDirEntry = pe->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
    if (exportDirEntry.Size == 0) {
        return NULL; // No export directory
    }

    IMAGE_EXPORT_DIRECTORY* exportDir = (IMAGE_EXPORT_DIRECTORY*)(base + exportDirEntry.VirtualAddress);

    // Get pointers to the key tables
    uint32_t* names = (uint32_t*)(base + exportDir->AddressOfNames);
    uint32_t* functions = (uint32_t*)(base + exportDir->AddressOfFunctions);
    uint16_t* ordinals = (uint16_t*)(base + exportDir->AddressOfNameOrdinals);

    // Search for the function by name
    for (uint32_t i = 0; i < exportDir->NumberOfNames; i++) {
        const char* currentName = (const char*)(base + names[i]);
        if (stricmp(functionName, currentName) == 0) {
            // Name found, get the ordinal
            uint16_t ordinal = ordinals[i];
            // Use the ordinal to find the function's RVA
            uint32_t functionRva = functions[ordinal];
            // Return the absolute address
            return (void*)(base + functionRva);
        }
    }

    return NULL; // Function not found
}

void* LoadPeImage(void* buffer) {
    uint8_t* data = (uint8_t*)buffer;
    IMAGE_DOS_HEADER* dos = (IMAGE_DOS_HEADER*)data;
    if (dos->e_magic != IMAGE_DOS_SIGNATURE) {
        Print(L"Invalid MZ header\n");
        return NULL;
    }

    IMAGE_NT_HEADERS64* pe = (IMAGE_NT_HEADERS64*)(data + dos->e_lfanew);
    if (pe->Signature != IMAGE_NT_SIGNATURE) {
        Print(L"Invalid PE header\n");
        return NULL;
    }

    if (pe->OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR64_MAGIC) {
        Print(L"Not PE32+\n");
        return NULL;
    }

    // Allocate memory for the full image
    uint64_t imageSize = pe->OptionalHeader.SizeOfImage;
    uint64_t imageBase = (uint64_t)Malloc(imageSize);
    if (!imageBase) {
        Print(L"Failed to allocate image memory\n");
        return NULL;
    }

    ZeroMem((void*)imageBase, imageSize);

    // Copy headers
    CopyMem((void*)imageBase, data, pe->OptionalHeader.SizeOfHeaders);

    // Copy each section
    IMAGE_SECTION_HEADER* sections = (IMAGE_SECTION_HEADER*)((uint8_t*)&pe->OptionalHeader + pe->FileHeader.SizeOfOptionalHeader);
    for (uint64_t i = 0; i < pe->FileHeader.NumberOfSections; i++) {
        void* dest = (void*)(imageBase + sections[i].VirtualAddress);
        void* src = (void*)(data + sections[i].PointerToRawData);
        uint64_t size = sections[i].SizeOfRawData;
        if (size > 0) {
            CopyMem(dest, src, size);
        }
    }

    // Relocation
    uint64_t imageDelta = imageBase - pe->OptionalHeader.ImageBase;
    if (imageDelta != 0 && pe->OptionalHeader.NumberOfRvaAndSizes > IMAGE_DIRECTORY_ENTRY_BASERELOC) {
        IMAGE_DATA_DIRECTORY relocDir = pe->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC];
        if (relocDir.Size > 0) {
            uint8_t* relocBase = (uint8_t*)(imageBase + relocDir.VirtualAddress);
            uint8_t* relocEnd = relocBase + relocDir.Size;

            while (relocBase < relocEnd) {
                IMAGE_BASE_RELOCATION* rel = (IMAGE_BASE_RELOCATION*)relocBase;
                if (rel->SizeOfBlock < sizeof(IMAGE_BASE_RELOCATION)) {
                    Print(L"Relocation block too small at %p\n", rel);
                    return NULL;
                }
                uint64_t entryCount = (rel->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) / sizeof(uint16_t);
                uint16_t* relocs = (uint16_t*)(rel + 1);

                for (uint64_t i = 0; i < entryCount; i++) {
                    uint16_t entry = relocs[i];
                    uint16_t type = entry >> 12;
                    uint16_t offset = entry & 0xFFF;

                    if (type == IMAGE_REL_BASED_DIR64) {
                        uint64_t* patchAddr = (uint64_t*)(imageBase + rel->VirtualAddress + offset);
                        // Check patchAddr is within image
                        if ((uintptr_t)patchAddr < imageBase || (uintptr_t)patchAddr >= imageBase + imageSize) {
                            Print(L"Relocation address out of bounds: %p\n", patchAddr);
                            return NULL;
                        }
                        *patchAddr += imageDelta;
                    } else if (type != 0) {
                        Print(L"Unknown relocation type %u at RVA %x\n", type, (unsigned)(rel->VirtualAddress + offset));
                        return NULL;
                    }
                }
                relocBase += rel->SizeOfBlock;
            }
        }
    }
    
    if (pe->OptionalHeader.NumberOfRvaAndSizes > IMAGE_DIRECTORY_ENTRY_IMPORT) {
        IMAGE_DATA_DIRECTORY importDirEntry = pe->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
        if (importDirEntry.Size > 0) {
            IMAGE_IMPORT_DESCRIPTOR* importDesc = (IMAGE_IMPORT_DESCRIPTOR*)(imageBase + importDirEntry.VirtualAddress);

            // Iterate over each DLL descriptor
            while (importDesc->Name != 0) {
                const char* dllName = (const char*)(imageBase + importDesc->Name);
                IMAGE_THUNK_DATA64* thunkINT = (IMAGE_THUNK_DATA64*)(imageBase + importDesc->u1.OriginalFirstThunk);
                IMAGE_THUNK_DATA64* thunkIAT = (IMAGE_THUNK_DATA64*)(imageBase + importDesc->FirstThunk);

                // Iterate over each function in the DLL
                while (thunkINT->u1.AddressOfData != 0) {
                    void* funcPtr = NULL;
                    if (thunkINT->u1.Ordinal & IMAGE_ORDINAL_FLAG64) {
                        // Import by ordinal - your ResolveImport doesn't support this
                        Print(L"Import by ordinal is not supported.\n");
                        return NULL;
                    } else {
                        // Import by name
                        IMAGE_IMPORT_BY_NAME* importByName = (IMAGE_IMPORT_BY_NAME*)(imageBase + thunkINT->u1.AddressOfData);
                        const char* funcName = importByName->Name;
                        funcPtr = ResolveImport(dllName, funcName);

                        if (!funcPtr) {
                            Print(L"Failed to resolve import: %a -> %a\n", dllName, importByName->Name);
                            return NULL;
                        }
                    }

                    // Write the resolved address into the IAT
                    thunkIAT->u1.Function = (uint64_t)funcPtr;

                    thunkINT++;
                    thunkIAT++;
                }
                importDesc++; // Move to the next DLL
            }
        }
    }

    // Return base
    return (void*)imageBase;
}
