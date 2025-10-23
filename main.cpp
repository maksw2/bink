#include <efi.h>
#include <efilib.h>
#include <x86_64\pe.h>
#define malloc AllocatePool
#define free FreePool
#include <immintrin.h>
#include <compiler_defines.h>
#include "bink.h"
#include "print.h"

#pragma region

// Macro to define function pointer and global variable
#define DECL_FN(ret, name, args, ptr) typedef ret (__cdecl *name##_t) args; name##_t p##name = NULL;

DECL_FN(void *, BinkLogoAddress, (void), )
DECL_FN(void, BinkSetError, (char const * err), *)
DECL_FN(char *, BinkGetError, (void), )
DECL_FN(HBINK, BinkOpen, (char const * name, U32 flags), *)
DECL_FN(HBINK, BinkOpenWithOptions, (char const * name, BINK_OPEN_OPTIONS const * boo, U32 flags), *)
DECL_FN(void, BinkGetFrameBuffersInfo, (HBINK bink, BINKFRAMEBUFFERS * fbset), *)
DECL_FN(void, BinkRegisterFrameBuffers, (HBINK bink, BINKFRAMEBUFFERS * fbset), *)
DECL_FN(S32, BinkDoFrame, (HBINK bnk), *)
DECL_FN(S32, BinkDoFramePlane, (HBINK bnk, U32 which_planes), *)
DECL_FN(void, BinkNextFrame, (HBINK bnk), *)
DECL_FN(S32, BinkWait, (HBINK bnk), *)
DECL_FN(void, BinkClose, (HBINK bnk), *)
DECL_FN(S32, BinkPause, (HBINK bnk, S32 pause), *)
DECL_FN(S32, BinkCopyToBuffer, (HBINK bnk, void* dest, S32 destpitch, U32 destheight, U32 destx, U32 desty, U32 flags), *)
DECL_FN(S32, BinkCopyToBufferRect, (HBINK bnk, void* dest, S32 destpitch, U32 destheight, U32 destx, U32 desty, U32 srcx, U32 srcy, U32 srcw, U32 srch, U32 flags), *)
DECL_FN(S32, BinkGetRects, (HBINK bnk, U32 flags), *)
DECL_FN(void, BinkGoto, (HBINK bnk, U32 frame, S32 flags), *)
DECL_FN(U32, BinkGetKeyFrame, (HBINK bnk, U32 frame, S32 flags), *)
DECL_FN(void, BinkFreeGlobals, (void), *)
DECL_FN(S32, BinkGetPlatformInfo, (S32 bink_platform_enum, void * output), *)
DECL_FN(void, BinkRegisterGPUDataBuffers, (HBINK bink, BINKGPUDATABUFFERS * gpu), *)
DECL_FN(S32, BinkGetGPUDataBuffersInfo, (HBINK bink, BINKGPUDATABUFFERS * gpu), *)
DECL_FN(S32, BinkSetVideoOnOff, (HBINK bnk, S32 onoff), *)
DECL_FN(S32, BinkSetSoundOnOff, (HBINK bnk, S32 onoff), *)
DECL_FN(void, BinkSetVolume, (HBINK bnk, U32 trackid, S32 volume), *)
DECL_FN(void, BinkSetPan, (HBINK bnk, U32 trackid, S32 pan), *)
DECL_FN(void, BinkSetSpeakerVolumes, (HBINK bnk, U32 trackid, U32 * speaker_indexes, S32 * volumes, U32 total), *)
DECL_FN(void, BinkService, (HBINK bink), *)
DECL_FN(S32, BinkShouldSkip, (HBINK bink), *)
DECL_FN(S32, BinkControlBackgroundIO, (HBINK bink, U32 control), *)
DECL_FN(void, BinkSetWillLoop, (HBINK bink, S32 onoff), *)
DECL_FN(S32, BinkStartAsyncThread, (S32 thread_num, void const * param), *)
DECL_FN(S32, BinkDoFrameAsync, (HBINK bink, U32 thread_one, U32 thread_two), *)
DECL_FN(S32, BinkDoFrameAsyncMulti, (HBINK bink, U32* threads, S32 thread_count), *)
DECL_FN(S32, BinkDoFrameAsyncWait, (HBINK bink, S32 us), *)
DECL_FN(S32, BinkRequestStopAsyncThread, (S32 thread_num), *)
DECL_FN(S32, BinkWaitStopAsyncThread, (S32 thread_num), *)
DECL_FN(HBINKTRACK, BinkOpenTrack, (HBINK bnk, U32 trackindex), *)
DECL_FN(void, BinkCloseTrack, (HBINKTRACK bnkt), *)
DECL_FN(U32, BinkGetTrackData, (HBINKTRACK bnkt, void * dest), *)
DECL_FN(U32, BinkGetTrackType, (HBINK bnk, U32 trackindex), *)
DECL_FN(U32, BinkGetTrackMaxSize, (HBINK bnk, U32 trackindex), *)
DECL_FN(U32, BinkGetTrackID, (HBINK bnk, U32 trackindex), *)
DECL_FN(void, BinkGetSummary, (HBINK bnk, BINKSUMMARY * sum), *)
DECL_FN(void, BinkGetRealtime, (HBINK bink, BINKREALTIME * run, U32 frames), *)
DECL_FN(void, BinkSetFileOffset, (U64 offset), *)
DECL_FN(void, BinkSetSoundTrack, (U32 total_tracks, U32 * tracks), *)
DECL_FN(void, BinkSetIO, (BINKIOOPEN io), *)
DECL_FN(void, BinkSetFrameRate, (U32 forcerate, U32 forceratediv), *)
DECL_FN(void, BinkSetSimulate, (U32 sim), *)
DECL_FN(void, BinkSetIOSize, (U32 iosize), *)
DECL_FN(S32, BinkSetSoundSystem, (BINKSNDSYSOPEN open, UINTa param), *)
DECL_FN(S32, BinkSetSoundSystem2, (BINKSNDSYSOPEN2 open, UINTa param1, UINTa param2), *)
DECL_FN(BINKSNDOPEN, BinkOpenDirectSound, (UINTa param), *)
DECL_FN(BINKSNDOPEN, BinkOpenWaveOut, (UINTa param), *)
DECL_FN(BINKSNDOPEN, BinkOpenXAudio2, (UINTa param), *)
DECL_FN(void, BinkSetMemory, (BINKMEMALLOC a, BINKMEMFREE f), *)

#pragma endregion
#pragma region

EFI_GRAPHICS_OUTPUT_PROTOCOL* gop;
EFI_GRAPHICS_OUTPUT_BLT_PIXEL* framebuffer;
EFI_GRAPHICS_OUTPUT_MODE_INFORMATION* mode_info;

#define SIMD_ALIGNMENT_BYTES 32
#define ALIGN_MASK (~(SIMD_ALIGNMENT_BYTES - 1))
#define ALIGN_CAST(ptr) ((UINTN)(ptr))

void* Malloc(size_t size) {
    PRINT(L"Malloc called with size: %u\n", size);

    VOID    *OriginalPtr;
    VOID    **AlignedPtrLoc;
    UINTN   TotalSize;
    UINTN   Addr;

    // Calculate total size:
    // 1. User requested size
    // 2. Maximum space for alignment padding (SIMD_ALIGNMENT_BYTES - 1)
    // 3. Space to store the original pointer for freeing (sizeof(VOID*))
    TotalSize = size + SIMD_ALIGNMENT_BYTES - 1 + sizeof(VOID*);

    // 1. Allocate a larger block of memory using the UEFI Pool service.
    if (EFI_ERROR(gBS->AllocatePool(EfiLoaderData, TotalSize, &OriginalPtr))) {
    return NULL; // Allocation failed
    }

    // 2. Calculate the aligned address.
    // Start the alignment calculation *after* the space reserved for the original pointer.
    Addr = ALIGN_CAST(OriginalPtr) + sizeof(VOID*);

    // Align the address up to the nearest SIMD_ALIGNMENT_BYTES multiple:
    // (Addr + Alignment - 1) & ~(Alignment - 1)
    Addr = (Addr + SIMD_ALIGNMENT_BYTES - 1) & ALIGN_MASK;

    // 3. Store the original pointer (OriginalPtr) immediately before the aligned address (Addr).
    // The location is 8 bytes (sizeof(VOID*)) behind the start of the aligned block.
    AlignedPtrLoc = (VOID**)(Addr - sizeof(VOID*));
    *AlignedPtrLoc = OriginalPtr;

    // 4. Return the aligned address to the caller.
    return (VOID*)Addr;
}

void Free(void* ptr) {
    PRINT(L"Free called with ptr: %p\n", ptr);

    VOID **OriginalPtrLoc;
    VOID *OriginalPtr;

    if (ptr == NULL) {
    return;
    }

    // 1. Calculate the address where the original pointer is stored.
    // It is located sizeof(VOID*) bytes immediately before the AlignedPtr.
    OriginalPtrLoc = (VOID**)(ALIGN_CAST(ptr) - sizeof(VOID*));
    OriginalPtr    = *OriginalPtrLoc;

    // 2. Use the UEFI Boot Service to free the original, unaligned block.
    gBS->FreePool(OriginalPtr);
}

EFI_STATUS InitializeGop() {
    EFI_STATUS Status;

    // 1. Locate the Graphics Output Protocol
    Status = LibLocateProtocol(&gEfiGraphicsOutputProtocolGuid, (VOID**)&gop);
    if (EFI_ERROR(Status)) {
        Print(L"Failed to locate Graphics Output Protocol: %r\n", Status);
        return Status;
    }

    // 2. Get framebuffer information
    mode_info = gop->Mode->Info;
    framebuffer = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL*)gop->Mode->FrameBufferBase; // yeah yeah blah blah

    return EFI_SUCCESS;
}

EFI_FILE_HANDLE GetVolume(EFI_HANDLE ImageHandle)
{
    EFI_LOADED_IMAGE* loaded_image = NULL;                  /* image interface */
    EFI_GUID lipGuid = EFI_LOADED_IMAGE_PROTOCOL_GUID;      /* image interface GUID */
    EFI_FILE_IO_INTERFACE* IOVolume;                        /* file system interface */
    EFI_GUID fsGuid = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID; /* file system interface GUID */
    EFI_FILE_HANDLE Volume;                                 /* the volume's interface */

    /* get the loaded image protocol interface for our "image" */
    BS->HandleProtocol(ImageHandle, &lipGuid, (void**)&loaded_image);

    /* get the volume handle */
    BS->HandleProtocol(loaded_image->DeviceHandle, &fsGuid, (VOID**)&IOVolume);
    IOVolume->OpenVolume(IOVolume, &Volume);
    return Volume;
}

UINT64 FileSize(EFI_FILE_HANDLE FileHandle) {
    EFI_FILE_INFO* FileInfo = LibFileInfo(FileHandle);
    UINT64 size = FileInfo->FileSize;
    FreePool(FileInfo);
    return size;
}

void* LoadFile(EFI_FILE_HANDLE volume, const wchar_t* filename) {
    EFI_FILE_HANDLE file;
    EFI_STATUS file_loading_status = volume->Open(volume, &file, (wchar_t*)filename,
        EFI_FILE_MODE_READ, 0);
    if (EFI_ERROR(file_loading_status)) {
        Print(L"Failed to open file: %r\n", file_loading_status);
        return NULL;
    }

    // Get the file size
    UINT64 file_size = FileSize(file);
    if (file_size == 0) {
        file->Close(file);
        return NULL;
    }

    VOID* file_buffer = Malloc(file_size);
    if (!file_buffer) { return NULL; }

    SetMem(file_buffer, file_size, 0);

    // Read the file into the allocated buffer
    UINTN read_size = (UINTN)file_size;
    file_loading_status = file->Read(file, &read_size, file_buffer);
    file->Close(file);

    if (EFI_ERROR(file_loading_status)) {
        Print(L"Failed to read file: %r\n", file_loading_status);
        // Free the allocated memory if the read fails
        Free(file_buffer);
        return NULL;
    }

    if (read_size != file_size) {
        Print(L"Read size mismatch: expected %llu, got %llu\n", file_size, read_size);
        Free(file_buffer);
        return NULL;
    }

    return file_buffer;
}

#pragma endregion
#pragma region

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

bool LoadBinkDLL(EFI_FILE_HANDLE volume) {
    void* binkFileBuffer = LoadFile(volume, L"bink2w64.dll");
    if (!binkFileBuffer) {
        return false;
    }

    void* binkBase = LoadPeImage(binkFileBuffer);
    Free(binkFileBuffer); // Free the file buffer, as LoadPeImage copied it.
    if (!binkBase) {
        return false;
    }

    // Grab exports
    #define GRAB(name) p##name = (name##_t)GetExportByName(binkBase, #name); \
                       if (!p##name) { Print(L"Missing export: %a\n", #name); return false; }

    GRAB(BinkLogoAddress)
    GRAB(BinkSetError)
    GRAB(BinkGetError)
    GRAB(BinkOpen)
    GRAB(BinkOpenWithOptions)
    GRAB(BinkGetFrameBuffersInfo)
    GRAB(BinkRegisterFrameBuffers)
    GRAB(BinkDoFrame)
    GRAB(BinkDoFramePlane)
    GRAB(BinkNextFrame)
    GRAB(BinkWait)
    GRAB(BinkClose)
    GRAB(BinkPause)
    GRAB(BinkCopyToBuffer)
    GRAB(BinkCopyToBufferRect)
    GRAB(BinkGetRects)
    GRAB(BinkGoto)
    GRAB(BinkGetKeyFrame)
    GRAB(BinkFreeGlobals)
    GRAB(BinkGetPlatformInfo)
    GRAB(BinkRegisterGPUDataBuffers)
    GRAB(BinkGetGPUDataBuffersInfo)
    GRAB(BinkSetVideoOnOff)
    GRAB(BinkSetSoundOnOff)
    GRAB(BinkSetVolume)
    GRAB(BinkSetPan)
    GRAB(BinkSetSpeakerVolumes)
    GRAB(BinkService)
    GRAB(BinkShouldSkip)
    GRAB(BinkControlBackgroundIO)
    GRAB(BinkSetWillLoop)
    GRAB(BinkStartAsyncThread)
    GRAB(BinkDoFrameAsync)
    GRAB(BinkDoFrameAsyncMulti)
    GRAB(BinkDoFrameAsyncWait)
    GRAB(BinkRequestStopAsyncThread)
    GRAB(BinkWaitStopAsyncThread)
    GRAB(BinkOpenTrack)
    GRAB(BinkCloseTrack)
    GRAB(BinkGetTrackData)
    GRAB(BinkGetTrackType)
    GRAB(BinkGetTrackMaxSize)
    GRAB(BinkGetTrackID)
    GRAB(BinkGetSummary)
    GRAB(BinkGetRealtime)
    GRAB(BinkSetFileOffset)
    GRAB(BinkSetSoundTrack)
    GRAB(BinkSetIO)
    GRAB(BinkSetFrameRate)
    GRAB(BinkSetSimulate)
    GRAB(BinkSetIOSize)
    GRAB(BinkSetSoundSystem)
    GRAB(BinkSetSoundSystem2)
    GRAB(BinkOpenDirectSound)
    GRAB(BinkOpenWaveOut)
    GRAB(BinkOpenXAudio2)
    GRAB(BinkSetMemory)

    #undef GRAB
    return true;
}
#pragma endregion

void Blit1280x720_AVX2_NT(
    EFI_GRAPHICS_OUTPUT_PROTOCOL *Gop,
    uint32_t *src, 
    UINTN src_width, UINTN src_height
) {
    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *info = Gop->Mode->Info;
    UINTN fb_width  = info->HorizontalResolution;
    UINTN fb_height = info->VerticalResolution;
    UINTN stride    = info->PixelsPerScanLine;

    uint32_t *fb = (uint32_t *)Gop->Mode->FrameBufferBase;

    // Center offsets
    UINTN x0 = (fb_width  - src_width) / 2;
    UINTN y0 = (fb_height - src_height) / 2;

    for (UINTN y = 0; y < src_height; y++) {
        uint32_t *dst = fb + (y0 + y) * stride + x0;
        uint32_t *s   = src + y * src_width;

        UINTN x = 0;

        // Align destination pointer to 32-byte boundary for AVX2 streaming
        for (; ((uintptr_t)(dst + x) & 31) && x < src_width; x++)
            dst[x] = s[x];

        // Copy 8 pixels (32 bytes) at a time using AVX2 non-temporal stores
        for (; x + 7 < src_width; x += 8) {
            __m256i data = _mm256_loadu_si256((__m256i *)(s + x));
            _mm256_stream_si256((__m256i *)(dst + x), data);
        }

        // Copy any remaining pixels
        for (; x < src_width; x++)
            dst[x] = s[x];
    }

    // Ensure non-temporal stores are flushed to memory
    _mm_sfence();
}

EFI_STATUS SetMaxResolution(EFI_GRAPHICS_OUTPUT_PROTOCOL *Gop) {
    EFI_STATUS Status;
    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *MaxModeInfo;
    UINTN MaxModeNumber;

    // Iterate through all available modes to find the one with the maximum resolution
    MaxModeNumber = 0;
    MaxModeInfo = NULL;
    for (UINTN i = 0; i < Gop->Mode->MaxMode; i++) {
        EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *Info;
        UINTN SizeOfInfo;

        Status = Gop->QueryMode(Gop, i, &SizeOfInfo, &Info);
        if (EFI_ERROR(Status)) {
            continue;
        }

        // Compare resolutions
        if (Info->HorizontalResolution > Gop->Mode->Info->HorizontalResolution ||
            (Info->HorizontalResolution == Gop->Mode->Info->HorizontalResolution && Info->VerticalResolution > Gop->Mode->Info->VerticalResolution)) {
            MaxModeNumber = i;
            MaxModeInfo = Info;
        }
    }

    if (MaxModeInfo) {
        Print(L"Found max resolution mode: %u x %u at mode %u\n",
              MaxModeInfo->HorizontalResolution, MaxModeInfo->VerticalResolution, MaxModeNumber);
        
        // Set the mode to the maximum resolution
        Status = Gop->SetMode(Gop, MaxModeNumber);
        if (EFI_ERROR(Status)) {
            Print(L"Failed to set mode: %r\n", Status);
        } else {
            Print(L"Successfully set max resolution mode.\n");
        }
        FreePool(MaxModeInfo); // Free the memory for MaxModeInfo
    } else {
        Print(L"Could not find a valid mode.\n");
        Status = EFI_NOT_FOUND;
    }
    
    return Status;
}

extern "C" void enable_avx2();

EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable) {
    InitializeLib(ImageHandle, SystemTable);
    SystemTable->BootServices->SetWatchdogTimer(0, 0, 0, NULL);
    Print(L"Hello, World!\n");

    InitializeGop();
    Print(L"GOP initialized\n");

    SetMaxResolution(gop);

    enable_avx2();

    EFI_FILE_HANDLE volume = GetVolume(ImageHandle);
    Print(L"Got volume\n");

    if (!LoadBinkDLL(volume)) {
        Print(L"Failed to load Bink Video DLL\n");
        gBS->Stall(5 * 1000 * 1000);
        gST->RuntimeServices->ResetSystem(EfiResetShutdown, EFI_SUCCESS, 0, NULL);
        return 1;
    }

    Print(L"Bink function pointers initialized (propably)!\n");

    //pBinkSetSoundSystem(pBinkOpenWaveOut, 0);
    pBinkSetMemory((BINKMEMALLOC)Malloc, (BINKMEMFREE)Free); // debug allocators
    pBinkSetIO(0); // no io

    Print(L"BinkSet* called\n");

    void* video_file = LoadFile(volume, L"video.bk2");

    Print(L"Video file loaded into memory\n");

    HBINK bink = NULL;
    bink = pBinkOpen((const char*)video_file, BINKFROMMEMORY | BINKNOSKIP | BINKNOTHREADEDIO);
    Print(L"Opened Bink!\n");

    int width = bink->Width, height = bink->Height;
    Print(L"Width: %d, Height: %d\n", width, height);

    // Allocate framebuffer
    uint32_t* framebuffer = (uint32_t*)Malloc(width * height * sizeof(uint32_t));
    if (!framebuffer) {
        Print(L"Failed to allocate framebuffer!\n");
        gBS->Stall(5 * 1000 * 1000);
        pBinkClose(bink);
        Free(video_file);
        return 1;
    }

    Print(L"Framebuffer allocated!\n");

    size_t skipped_frames = 0;

    // start the decode loop
    while (true) {
        // Check if the video is done
        if (bink->FrameNum >= bink->Frames) {
            Print(L"\nVideo playback finished!\n");
            break; // exit the loop if the video is done
        }
        
        // Wait for the next frame
        if (pBinkWait(bink)) {
            _mm_pause(); // don't burn CPU cycles
            continue; // wait for the next frame
        }

        // Check if we should skip the frame
        if (pBinkShouldSkip(bink)) {
            pBinkNextFrame(bink);
            skipped_frames++;
            continue; // skip this frame
        }

        int code = pBinkDoFrame(bink);
        if (code != 0) {
            Print(L"BinkDoFrame failed!\n");
            pBinkClose(bink);
            Free(video_file);
            gBS->Stall(5 * 1000 * 1000);
            gST->RuntimeServices->ResetSystem(EfiResetShutdown, EFI_SUCCESS, 0, NULL);
            return 1;
        }

        // Copy frame to our buffer
        code = pBinkCopyToBuffer(
            bink,
            framebuffer,
            width * 4,
            height,
            0,
            0,
            BINKSURFACE32 | BINKCOPYALL
        );
        if (code != 0 && code != 1) {
            Print(L"BinkCopyToBuffer failed!\n");
            pBinkClose(bink);
            Free(video_file);
            gBS->Stall(5 * 1000 * 1000);
            gST->RuntimeServices->ResetSystem(EfiResetShutdown, EFI_SUCCESS, 0, NULL);
            return 1;
        }
        
        pBinkNextFrame(bink);

        Blit1280x720_AVX2_NT(gop, framebuffer, width, height);

        Print(L"\rframe %d done", bink->FrameNum);
    }

    Print(L"Skipped frames: %u\n", skipped_frames);

    pBinkClose(bink);

    Free(video_file);
    Free(framebuffer);

    Print(L"Exiting...\n");
    gBS->Stall(5 * 1000 * 1000);
    gST->RuntimeServices->ResetSystem(EfiResetShutdown, EFI_SUCCESS, 0, NULL);

    return 0;
}
