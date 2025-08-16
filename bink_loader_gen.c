#include "bink.h"
#include <windows.h>

// Macro to define function pointer and global variable
#define DECL_FN(ret, name, args, ptr) typedef ret (__cdecl *name##_t) args; name##_t p##name = NULL;

DECL_FN(RADEXPFUNC void *, BinkLogoAddress, (void), )
DECL_FN(RADEXPFUNC void, BinkSetError, (char const * err), *)
DECL_FN(RADEXPFUNC char *, BinkGetError, (void), )
DECL_FN(RADEXPFUNC HBINK, BinkOpen, (char const * name, U32 flags), *)
DECL_FN(RADEXPFUNC HBINK, BinkOpenWithOptions, (char const * name, BINK_OPEN_OPTIONS const * boo, U32 flags), *)
DECL_FN(RADEXPFUNC void, BinkGetFrameBuffersInfo, (HBINK bink, BINKFRAMEBUFFERS * fbset), *)
DECL_FN(RADEXPFUNC void, BinkRegisterFrameBuffers, (HBINK bink, BINKFRAMEBUFFERS * fbset), *)
DECL_FN(RADEXPFUNC S32, BinkDoFrame, (HBINK bnk), *)
DECL_FN(RADEXPFUNC S32, BinkDoFramePlane, (HBINK bnk, U32 which_planes), *)
DECL_FN(RADEXPFUNC void, BinkNextFrame, (HBINK bnk), *)
DECL_FN(RADEXPFUNC S32, BinkWait, (HBINK bnk), *)
DECL_FN(RADEXPFUNC void, BinkClose, (HBINK bnk), *)
DECL_FN(RADEXPFUNC S32, BinkPause, (HBINK bnk, S32 pause), *)
DECL_FN(RADEXPFUNC S32, BinkCopyToBuffer, (HBINK bnk, void* dest, S32 destpitch, U32 destheight, U32 destx, U32 desty, U32 flags), *)
DECL_FN(RADEXPFUNC S32, BinkCopyToBufferRect, (HBINK bnk, void* dest, S32 destpitch, U32 destheight, U32 destx, U32 desty, U32 srcx, U32 srcy, U32 srcw, U32 srch, U32 flags), *)
DECL_FN(RADEXPFUNC S32, BinkGetRects, (HBINK bnk, U32 flags), *)
DECL_FN(RADEXPFUNC void, BinkGoto, (HBINK bnk, U32 frame, S32 flags), *)
DECL_FN(RADEXPFUNC U32, BinkGetKeyFrame, (HBINK bnk, U32 frame, S32 flags), *)
DECL_FN(RADEXPFUNC void, BinkFreeGlobals, (void), *)
DECL_FN(RADEXPFUNC S32, BinkGetPlatformInfo, (S32 bink_platform_enum, void * output), *)
DECL_FN(RADEXPFUNC void, BinkRegisterGPUDataBuffers, (HBINK bink, BINKGPUDATABUFFERS * gpu), *)
DECL_FN(RADEXPFUNC S32, BinkGetGPUDataBuffersInfo, (HBINK bink, BINKGPUDATABUFFERS * gpu), *)
DECL_FN(RADEXPFUNC S32, BinkSetVideoOnOff, (HBINK bnk, S32 onoff), *)
DECL_FN(RADEXPFUNC S32, BinkSetSoundOnOff, (HBINK bnk, S32 onoff), *)
DECL_FN(RADEXPFUNC void, BinkSetVolume, (HBINK bnk, U32 trackid, S32 volume), *)
DECL_FN(RADEXPFUNC void, BinkSetPan, (HBINK bnk, U32 trackid, S32 pan), *)
DECL_FN(RADEXPFUNC void, BinkSetSpeakerVolumes, (HBINK bnk, U32 trackid, U32 * speaker_indexes, S32 * volumes, U32 total), *)
DECL_FN(RADEXPFUNC void, BinkService, (HBINK bink), *)
DECL_FN(RADEXPFUNC S32, BinkShouldSkip, (HBINK bink), *)
DECL_FN(RADEXPFUNC S32, BinkControlBackgroundIO, (HBINK bink, U32 control), *)
DECL_FN(RADEXPFUNC void, BinkSetWillLoop, (HBINK bink, S32 onoff), *)
DECL_FN(RADEXPFUNC S32, BinkStartAsyncThread, (S32 thread_num, void const * param), *)
DECL_FN(RADEXPFUNC S32, BinkDoFrameAsync, (HBINK bink, U32 thread_one, U32 thread_two), *)
DECL_FN(RADEXPFUNC S32, BinkDoFrameAsyncMulti, (HBINK bink, U32* threads, S32 thread_count), *)
DECL_FN(RADEXPFUNC S32, BinkDoFrameAsyncWait, (HBINK bink, S32 us), *)
DECL_FN(RADEXPFUNC S32, BinkRequestStopAsyncThread, (S32 thread_num), *)
DECL_FN(RADEXPFUNC S32, BinkWaitStopAsyncThread, (S32 thread_num), *)
DECL_FN(RADEXPFUNC HBINKTRACK, BinkOpenTrack, (HBINK bnk, U32 trackindex), *)
DECL_FN(RADEXPFUNC void, BinkCloseTrack, (HBINKTRACK bnkt), *)
DECL_FN(RADEXPFUNC U32, BinkGetTrackData, (HBINKTRACK bnkt, void * dest), *)
DECL_FN(RADEXPFUNC U32, BinkGetTrackType, (HBINK bnk, U32 trackindex), *)
DECL_FN(RADEXPFUNC U32, BinkGetTrackMaxSize, (HBINK bnk, U32 trackindex), *)
DECL_FN(RADEXPFUNC U32, BinkGetTrackID, (HBINK bnk, U32 trackindex), *)
DECL_FN(RADEXPFUNC void, BinkGetSummary, (HBINK bnk, BINKSUMMARY * sum), *)
DECL_FN(RADEXPFUNC void, BinkGetRealtime, (HBINK bink, BINKREALTIME * run, U32 frames), *)
DECL_FN(RADEXPFUNC void, BinkSetFileOffset, (U64 offset), *)
DECL_FN(RADEXPFUNC void, BinkSetSoundTrack, (U32 total_tracks, U32 * tracks), *)
DECL_FN(RADEXPFUNC void, BinkSetIO, (BINKIOOPEN io), *)
DECL_FN(RADEXPFUNC void, BinkSetFrameRate, (U32 forcerate, U32 forceratediv), *)
DECL_FN(RADEXPFUNC void, BinkSetSimulate, (U32 sim), *)
DECL_FN(RADEXPFUNC void, BinkSetIOSize, (U32 iosize), *)
DECL_FN(RADEXPFUNC S32, BinkSetSoundSystem, (BINKSNDSYSOPEN open, UINTa param), *)
DECL_FN(RADEXPFUNC S32, BinkSetSoundSystem2, (BINKSNDSYSOPEN2 open, UINTa param1, UINTa param2), *)
DECL_FN(RADEXPFUNC BINKSNDOPEN, BinkOpenDirectSound, (UINTa param), *)
DECL_FN(RADEXPFUNC BINKSNDOPEN, BinkOpenWaveOut, (UINTa param), *)
DECL_FN(RADEXPFUNC BINKSNDOPEN, BinkOpenXAudio2, (UINTa param), *)
DECL_FN(RADEXPFUNC void, BinkSetMemory, (BINKMEMALLOC a, BINKMEMFREE f), *)

HMODULE dll = NULL;

int LoadBinkDLL(void) {
    if (dll) return 1;
    dll = LoadLibraryA("bink2w64.dll");
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
