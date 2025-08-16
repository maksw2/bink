#include "bink.h"
#include <windows.h>
#include <stdio.h>
#include <stdint.h>

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

HMODULE dll = NULL;

int LoadBinkDLL(void) {
    if (dll) return 1;
    dll = LoadLibraryExA("bink2w64.dll", 0, 0);
    if (!dll) return 0;

    pBinkLogoAddress = (BinkLogoAddress_t)GetProcAddress(dll, "BinkLogoAddress");
    pBinkSetError = (BinkSetError_t)GetProcAddress(dll, "BinkSetError");
    pBinkGetError = (BinkGetError_t)GetProcAddress(dll, "BinkGetError");
    pBinkOpen = (BinkOpen_t)GetProcAddress(dll, "BinkOpen");
    pBinkOpenWithOptions = (BinkOpenWithOptions_t)GetProcAddress(dll, "BinkOpenWithOptions");
    pBinkGetFrameBuffersInfo = (BinkGetFrameBuffersInfo_t)GetProcAddress(dll, "BinkGetFrameBuffersInfo");
    pBinkRegisterFrameBuffers = (BinkRegisterFrameBuffers_t)GetProcAddress(dll, "BinkRegisterFrameBuffers");
    pBinkDoFrame = (BinkDoFrame_t)GetProcAddress(dll, "BinkDoFrame");
    pBinkDoFramePlane = (BinkDoFramePlane_t)GetProcAddress(dll, "BinkDoFramePlane");
    pBinkNextFrame = (BinkNextFrame_t)GetProcAddress(dll, "BinkNextFrame");
    pBinkWait = (BinkWait_t)GetProcAddress(dll, "BinkWait");
    pBinkClose = (BinkClose_t)GetProcAddress(dll, "BinkClose");
    pBinkPause = (BinkPause_t)GetProcAddress(dll, "BinkPause");
    pBinkCopyToBuffer = (BinkCopyToBuffer_t)GetProcAddress(dll, "BinkCopyToBuffer");
    pBinkCopyToBufferRect = (BinkCopyToBufferRect_t)GetProcAddress(dll, "BinkCopyToBufferRect");
    pBinkGetRects = (BinkGetRects_t)GetProcAddress(dll, "BinkGetRects");
    pBinkGoto = (BinkGoto_t)GetProcAddress(dll, "BinkGoto");
    pBinkGetKeyFrame = (BinkGetKeyFrame_t)GetProcAddress(dll, "BinkGetKeyFrame");
    pBinkFreeGlobals = (BinkFreeGlobals_t)GetProcAddress(dll, "BinkFreeGlobals");
    pBinkGetPlatformInfo = (BinkGetPlatformInfo_t)GetProcAddress(dll, "BinkGetPlatformInfo");
    pBinkRegisterGPUDataBuffers = (BinkRegisterGPUDataBuffers_t)GetProcAddress(dll, "BinkRegisterGPUDataBuffers");
    pBinkGetGPUDataBuffersInfo = (BinkGetGPUDataBuffersInfo_t)GetProcAddress(dll, "BinkGetGPUDataBuffersInfo");
    pBinkSetVideoOnOff = (BinkSetVideoOnOff_t)GetProcAddress(dll, "BinkSetVideoOnOff");
    pBinkSetSoundOnOff = (BinkSetSoundOnOff_t)GetProcAddress(dll, "BinkSetSoundOnOff");
    pBinkSetVolume = (BinkSetVolume_t)GetProcAddress(dll, "BinkSetVolume");
    pBinkSetPan = (BinkSetPan_t)GetProcAddress(dll, "BinkSetPan");
    pBinkSetSpeakerVolumes = (BinkSetSpeakerVolumes_t)GetProcAddress(dll, "BinkSetSpeakerVolumes");
    pBinkService = (BinkService_t)GetProcAddress(dll, "BinkService");
    pBinkShouldSkip = (BinkShouldSkip_t)GetProcAddress(dll, "BinkShouldSkip");
    pBinkControlBackgroundIO = (BinkControlBackgroundIO_t)GetProcAddress(dll, "BinkControlBackgroundIO");
    pBinkSetWillLoop = (BinkSetWillLoop_t)GetProcAddress(dll, "BinkSetWillLoop");
    pBinkStartAsyncThread = (BinkStartAsyncThread_t)GetProcAddress(dll, "BinkStartAsyncThread");
    pBinkDoFrameAsync = (BinkDoFrameAsync_t)GetProcAddress(dll, "BinkDoFrameAsync");
    pBinkDoFrameAsyncMulti = (BinkDoFrameAsyncMulti_t)GetProcAddress(dll, "BinkDoFrameAsyncMulti");
    pBinkDoFrameAsyncWait = (BinkDoFrameAsyncWait_t)GetProcAddress(dll, "BinkDoFrameAsyncWait");
    pBinkRequestStopAsyncThread = (BinkRequestStopAsyncThread_t)GetProcAddress(dll, "BinkRequestStopAsyncThread");
    pBinkWaitStopAsyncThread = (BinkWaitStopAsyncThread_t)GetProcAddress(dll, "BinkWaitStopAsyncThread");
    pBinkOpenTrack = (BinkOpenTrack_t)GetProcAddress(dll, "BinkOpenTrack");
    pBinkCloseTrack = (BinkCloseTrack_t)GetProcAddress(dll, "BinkCloseTrack");
    pBinkGetTrackData = (BinkGetTrackData_t)GetProcAddress(dll, "BinkGetTrackData");
    pBinkGetTrackType = (BinkGetTrackType_t)GetProcAddress(dll, "BinkGetTrackType");
    pBinkGetTrackMaxSize = (BinkGetTrackMaxSize_t)GetProcAddress(dll, "BinkGetTrackMaxSize");
    pBinkGetTrackID = (BinkGetTrackID_t)GetProcAddress(dll, "BinkGetTrackID");
    pBinkGetSummary = (BinkGetSummary_t)GetProcAddress(dll, "BinkGetSummary");
    pBinkGetRealtime = (BinkGetRealtime_t)GetProcAddress(dll, "BinkGetRealtime");
    pBinkSetFileOffset = (BinkSetFileOffset_t)GetProcAddress(dll, "BinkSetFileOffset");
    pBinkSetSoundTrack = (BinkSetSoundTrack_t)GetProcAddress(dll, "BinkSetSoundTrack");
    pBinkSetIO = (BinkSetIO_t)GetProcAddress(dll, "BinkSetIO");
    pBinkSetFrameRate = (BinkSetFrameRate_t)GetProcAddress(dll, "BinkSetFrameRate");
    pBinkSetSimulate = (BinkSetSimulate_t)GetProcAddress(dll, "BinkSetSimulate");
    pBinkSetIOSize = (BinkSetIOSize_t)GetProcAddress(dll, "BinkSetIOSize");
    pBinkSetSoundSystem = (BinkSetSoundSystem_t)GetProcAddress(dll, "BinkSetSoundSystem");
    pBinkSetSoundSystem2 = (BinkSetSoundSystem2_t)GetProcAddress(dll, "BinkSetSoundSystem2");
    pBinkOpenDirectSound = (BinkOpenDirectSound_t)GetProcAddress(dll, "BinkOpenDirectSound");
    pBinkOpenWaveOut = (BinkOpenWaveOut_t)GetProcAddress(dll, "BinkOpenWaveOut");
    pBinkOpenXAudio2 = (BinkOpenXAudio2_t)GetProcAddress(dll, "BinkOpenXAudio2");
    pBinkSetMemory = (BinkSetMemory_t)GetProcAddress(dll, "BinkSetMemory");

    return 1;
}

char* read_binary_file_to_memory(const char* filename, size_t* file_size) {
    FILE* file = fopen(filename, "rb"); // Use "rb" for binary mode
    if (file == NULL) {
        perror("Failed to open file");
        return NULL;
    }

    // Get file size
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    rewind(file);

    if (size < 0) {
        perror("Failed to determine file size");
        fclose(file);
        return NULL;
    }

    // Allocate memory. Note: no need for a null terminator since it's not a string.
    char* buffer = (char*)malloc(size);
    if (buffer == NULL) {
        perror("Memory allocation failed");
        fclose(file);
        return NULL;
    }

    // Read file into buffer
    size_t bytes_read = fread(buffer, 1, size, file);
    if (bytes_read != (size_t)size) {
        perror("Failed to read file");
        free(buffer);
        fclose(file);
        return NULL;
    }

    fclose(file);

    if (file_size) {
        *file_size = (size_t)size;
    }

    return buffer;
}

int main() {
    if (!LoadBinkDLL()) {
        printf("we fucked up\n");
        return 1;
    }

    pBinkSetSoundSystem((BINKSNDSYSOPEN)pBinkOpenDirectSound, 0);
    pBinkSetMemory((BINKMEMALLOC)malloc, (BINKMEMFREE)free);
    pBinkSetIO(NULL); // no io
    //pBinkSetTimerRead() // r.i.p.

    size_t size = 0;
    char* buffer = read_binary_file_to_memory("you_are_nothing.bk2", &size);

    HBINK bink = NULL;
    bink = pBinkOpen(buffer, BINKFROMMEMORY);

    int width = bink->Width, height = bink->Height;

    // Allocate framebuffer
    uint32_t* framebuffer = (uint32_t*)malloc(width * height * sizeof(uint32_t));
    if (!framebuffer) return 1;

    // Decode one frame
    pBinkDoFrame(bink);

    // Copy frame to our buffer
    pBinkCopyToBuffer(
        bink,
        framebuffer,        // dest
        width * 4,          // destpitch in bytes
        height,             // destheight
        0,                  // destx
        0,                  // desty
        BINKSURFACE32 | BINKCOPYALL // flags
    );

    // Create BMP headers
    BITMAPFILEHEADER bfh = {0};
    BITMAPINFOHEADER bih = {0};

    bfh.bfType = 0x4D42; // 'BM'
    bfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    bfh.bfSize = bfh.bfOffBits + width * height * 4;

    bih.biSize = sizeof(BITMAPINFOHEADER);
    bih.biWidth = width;
    bih.biHeight = -height; // negative = top-down
    bih.biPlanes = 1;
    bih.biBitCount = 32;
    bih.biCompression = 0; // BI_RGB
    bih.biSizeImage = width * height * 4;

    // Write BMP
    FILE* f = fopen("framebuffer.bmp", "wb");
    if (!f) return 1;

    fwrite(&bfh, sizeof(bfh), 1, f);
    fwrite(&bih, sizeof(bih), 1, f);
    fwrite(framebuffer, 4, width * height, f);

    fclose(f);
    free(framebuffer);

    printf("Saved framebuffer.bmp\n");

    pBinkClose(bink);

    free(buffer);

    return 0;
}