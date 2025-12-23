#include <efi.h>
#include <efilib.h>
#include <intrin.h>
#include <immintrin.h>
#include <compiler_defines.h>
#include "print.h"
#include "take_care_of_bink.h"

int SerialPutc(char c) {
    // Define a timeout limit to prevent infinite hanging
    uint32_t timeout = 1000000; 

    // 0x3FD is the Line Status Register (LSR)
    // 0x20 is the Transmit Holding Register Empty (THRE) bit
    while (!(__inbyte(0x3FD) & 0x20)) {
        if (--timeout == 0) {
            return 0; // Hardware failure or busy timeout
        }
    }

    // 0x3F8 is the Transmit Holding Register (THR)
    __outbyte(0x3F8, (unsigned char)c);
    
    return 1;
}

void _putchar(char c) {
    SerialPutc(c);
}

EFI_GRAPHICS_OUTPUT_PROTOCOL* gop;
EFI_GRAPHICS_OUTPUT_MODE_INFORMATION* mode_info;

EFI_STATUS InitializeGop() {
    EFI_STATUS Status;

    // 1. Locate the Graphics Output Protocol
    Status = LibLocateProtocol(&gEfiGraphicsOutputProtocolGuid, (VOID**)&gop);
    if (EFI_ERROR(Status)) {
        printf("Failed to locate Graphics Output Protocol: %r\n", Status);
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
        printf("Failed to open file: %r\n", file_loading_status);
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
        printf("Failed to read file: %r\n", file_loading_status);
        // Free the allocated memory if the read fails
        Free(file_buffer);
        return NULL;
    }

    if (read_size != file_size) {
        printf("Read size mismatch: expected %llu, got %llu\n", file_size, read_size);
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
                       if (!p##name) { printf("Missing export: %s\n", #name); return false; }

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
        printf("Found max resolution mode: %u x %u at mode %u\n",
              MaxModeInfo->HorizontalResolution, MaxModeInfo->VerticalResolution, MaxModeNumber);
        
        // Set the mode to the maximum resolution
        Status = Gop->SetMode(Gop, MaxModeNumber);
        if (EFI_ERROR(Status)) {
            printf("Failed to set mode: %r\n", Status);
        } else {
            printf("Successfully set max resolution mode.\n");
        }
        FreePool(MaxModeInfo); // Free the memory for MaxModeInfo
    } else {
        printf("Could not find a valid mode.\n");
        Status = EFI_NOT_FOUND;
    }
    
    return Status;
}

extern "C" void enable_avx2();

#pragma pack(push, 1)
typedef struct {
    uint16_t type;
    uint32_t size;
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t offset;
    uint32_t dib_size;
    int32_t  width;
    int32_t  height;
    uint16_t planes;
    uint16_t bpp;
    uint32_t compression;
    uint32_t image_size;
    int32_t  x_ppm;
    int32_t  y_ppm;
    uint32_t colors;
    uint32_t important_colors;
} BMP_HEADER;
#pragma pack(pop)

void SavePhysicalScreen(EFI_FILE_HANDLE volume, EFI_GRAPHICS_OUTPUT_PROTOCOL* Gop) {
    UINT32 width = Gop->Mode->Info->HorizontalResolution;
    UINT32 height = Gop->Mode->Info->VerticalResolution;
    UINT32 stride = Gop->Mode->Info->PixelsPerScanLine;
    uint32_t* fb_base = (uint32_t*)Gop->Mode->FrameBufferBase;

    // 1. Allocate a single contiguous buffer for the BMP (Header + Pixels)
    UINTN total_pixel_bytes = width * height * 4;
    UINTN total_file_size = sizeof(BMP_HEADER) + total_pixel_bytes;
    uint8_t* file_buffer = (uint8_t*)AllocatePool(total_file_size);
    
    if (!file_buffer) {
        printf("Failed to allocate memory for screenshot!\n");
        return;
    }

    // 2. Setup Header
    BMP_HEADER* header = (BMP_HEADER*)file_buffer;
    SetMem(header, sizeof(BMP_HEADER), 0);
    header->type = 0x4D42;
    header->offset = sizeof(BMP_HEADER);
    header->size = (uint32_t)total_file_size;
    header->dib_size = 40;
    header->width = (int32_t)width;
    header->height = -(int32_t)height; // Top-down
    header->planes = 1;
    header->bpp = 32;

    // 3. Copy pixels to buffer while stripping Stride and fixing Alpha
    uint32_t* dst_pixels = (uint32_t*)(file_buffer + sizeof(BMP_HEADER));
    __m256i alpha_mask = _mm256_set1_epi32(0xFF000000); // Mask to force Alpha to 255

    for (UINT32 y = 0; y < height; y++) {
        uint32_t* src_row = fb_base + (y * stride);
        uint32_t* dst_row = dst_pixels + (y * width);

        UINT32 x = 0;
        // AVX2 Loop: Fix 8 pixels at a time
        for (; x + 7 < width; x += 8) {
            __m256i pixels = _mm256_loadu_si256((__m256i*)(src_row + x));
            pixels = _mm256_or_si256(pixels, alpha_mask); // Force Alpha channel to Opaque
            _mm256_storeu_si256((__m256i*)(dst_row + x), pixels);
        }
        // Cleanup loop for remaining pixels
        for (; x < width; x++) {
            dst_row[x] = src_row[x] | 0xFF000000;
        }
    }

    // 4. One single disk write (Massively faster)
    EFI_FILE_HANDLE file;
    EFI_STATUS status = volume->Open(volume, &file, (CHAR16*)L"screen.bmp", 
                                     EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE | EFI_FILE_MODE_CREATE, 0);
    if (!EFI_ERROR(status)) {
        UINTN write_size = total_file_size;
        file->Write(file, &write_size, file_buffer);
        file->Close(file);
    }

    FreePool(file_buffer);
}

EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable) {
    InitializeLib(ImageHandle, SystemTable);
    SystemTable->BootServices->SetWatchdogTimer(0, 0, 0, NULL);
    printf("Hello, World!\n");

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
        printf("Failed to allocate memory pool pages: %r\n", Status);
        gBS->Stall(5 * 1000 * 1000);
        gST->RuntimeServices->ResetSystem(EfiResetShutdown, EFI_SUCCESS, 0, NULL);
        return Status;
    }

    InitAllocatorFromBuffer(HeapMemory, AllocationSize);
    printf("Custom allocator initialized\n");

    InitializeGop();
    printf("GOP initialized\n");

    SetMaxResolution(gop);

    enable_avx2();

    EFI_FILE_HANDLE volume = GetVolume(ImageHandle);
    printf("Got volume\n");

    if (!LoadBinkDLL(volume)) {
        printf("Failed to load Bink Video DLL\n");
        gBS->Stall(5 * 1000 * 1000);
        gST->RuntimeServices->ResetSystem(EfiResetShutdown, EFI_SUCCESS, 0, NULL);
        return 1;
    }

    printf("Bink function pointers initialized!\n");

    //pBinkSetSoundSystem(pBinkOpenWaveOut, 0);
    pBinkSetMemory((BINKMEMALLOC)Malloc, (BINKMEMFREE)Free);
    pBinkSetIO(0); // no io

    printf("BinkSet* called\n");

    void* video_file = LoadFile(volume, L"idk.bk2");

    printf("Video file loaded into memory\n");

    HBINK bink = NULL;
    bink = pBinkOpen((const char*)video_file, BINKFROMMEMORY | BINKNOSKIP | BINKNOTHREADEDIO);
    printf("Opened Bink!\n");

    int width = bink->Width, height = bink->Height;
    printf("Width: %d, Height: %d\n", width, height);

    // Allocate framebuffer
    uint32_t* framebuffer = (uint32_t*)Malloc(width * height * sizeof(uint32_t));
    if (!framebuffer) {
        printf("Failed to allocate framebuffer!\n");
        gBS->Stall(5 * 1000 * 1000);
        pBinkClose(bink);
        Free(video_file);
        return 1;
    }

    printf("Framebuffer allocated!\n");

    EFI_INPUT_KEY key;
    bool paused = false;
    uint64_t skipped = 0;

    // start the decode loop
    while (true) {
        EFI_STATUS key_status = gST->ConIn->ReadKeyStroke(gST->ConIn, &key);
        if (key_status == EFI_SUCCESS) {
            if (key.UnicodeChar == 'q') {
                printf("\nExiting");
                break;
            }
            else if (key.UnicodeChar == 'r') {
                pBinkGoto(bink, 1, 0);
            }
            else if (key.UnicodeChar == 'p') {
                pBinkPause(bink, paused);
                paused = !paused;
            }
            else if (key.UnicodeChar == 's' ) {
                SavePhysicalScreen(volume, gop);
            }
        }

        // Check if the video is done
        if (bink->FrameNum >= bink->Frames) {
            printf("\nVideo playback finished!");
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
            skipped++;
            continue; // skip this frame
        }

        int code = pBinkDoFrame(bink);
        if (code != 0) {
            printf("BinkDoFrame failed!\n");
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
            printf("BinkCopyToBuffer failed!\n");
            pBinkClose(bink);
            Free(video_file);
            gBS->Stall(5 * 1000 * 1000);
            gST->RuntimeServices->ResetSystem(EfiResetShutdown, EFI_SUCCESS, 0, NULL);
            return 1;
        }
        
        pBinkNextFrame(bink);

        Blit_AVX2_NT(gop, framebuffer, width, height);

        if (bink->FrameNum % 10 == 0)
            printf("\rframe %d done    ", bink->FrameNum);
    }

    printf("\nTotal skipped frames: %llu\n", skipped);
    if (skipped > 100)
        printf("oof\n");

    pBinkClose(bink);

    Free(video_file);
    Free(framebuffer);

    printf("Exiting...\n");
    gBS->Stall(2 * 1000 * 1000);
    gST->RuntimeServices->ResetSystem(EfiResetShutdown, EFI_SUCCESS, 0, NULL);

    return 0;
}
