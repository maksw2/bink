#include <efi.h>
#include <efilib.h>
#include <immintrin.h>
#include <compiler_defines.h>
#include "print.h"
#include "take_care_of_bink.h"

EFI_GRAPHICS_OUTPUT_PROTOCOL* gop;
EFI_GRAPHICS_OUTPUT_MODE_INFORMATION* mode_info;

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

void Blit_AVX2_NT(
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

    VOID* HeapMemory = NULL;
    UINTN AllocationSize = 512 * 1024 * 1024; // 512 MB
    UINTN Pages = AllocationSize / 4096;      // UEFI page size is 4KB

    EFI_STATUS Status = gBS->AllocatePages(
        AllocateAnyPages,
        EfiLoaderData,
        Pages,
        (EFI_PHYSICAL_ADDRESS*)&HeapMemory
    );

    if (EFI_ERROR(Status)) {
        Print(L"Failed to allocate memory pool pages: %r\n", Status);
        gBS->Stall(5 * 1000 * 1000);
        gST->RuntimeServices->ResetSystem(EfiResetShutdown, EFI_SUCCESS, 0, NULL);
        return Status;
    }

    InitAllocatorFromBuffer(HeapMemory, AllocationSize);
    Print(L"Custom allocator initialized\n");

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

    Print(L"Bink function pointers initialized!\n");

    //pBinkSetSoundSystem(pBinkOpenWaveOut, 0);
    pBinkSetMemory((BINKMEMALLOC)Malloc, (BINKMEMFREE)Free);
    pBinkSetIO(0); // no io

    Print(L"BinkSet* called\n");

    void* video_file = LoadFile(volume, L"idk.bk2");

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

    EFI_INPUT_KEY key;
    bool paused = false;

    // start the decode loop
    while (true) {
        EFI_STATUS key_status = gST->ConIn->ReadKeyStroke(gST->ConIn, &key);
        if (key_status == EFI_SUCCESS) {
            if (key.UnicodeChar == 'q') {
                Print(L"\nExiting");
                break;
            }
            else if (key.UnicodeChar == 'r') {
                pBinkGoto(bink, 1, 0);
            }
            else if (key.UnicodeChar == 'p') {
                pBinkPause(bink, paused);
                paused = !paused;
            }
        }

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
        if (code != 0 && code != 1) { // who knows what they mean
            Print(L"BinkCopyToBuffer failed!\n");
            pBinkClose(bink);
            Free(video_file);
            gBS->Stall(5 * 1000 * 1000);
            gST->RuntimeServices->ResetSystem(EfiResetShutdown, EFI_SUCCESS, 0, NULL);
            return 1;
        }
        
        pBinkNextFrame(bink);

        Blit_AVX2_NT(gop, framebuffer, width, height);

        if (bink->FrameNum % 10 == 0)
            Print(L"\rframe %d done    ", bink->FrameNum);
    }

    pBinkClose(bink);

    Free(video_file);
    Free(framebuffer);

    Print(L"Exiting...\n");
    gBS->Stall(4 * 1000 * 1000);
    gST->RuntimeServices->ResetSystem(EfiResetShutdown, EFI_SUCCESS, 0, NULL);

    return 0;
}
