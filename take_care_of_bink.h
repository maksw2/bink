#pragma once
#include "bink.h"

void InitAllocatorFromBuffer(void* buffer, size_t size);
void* Malloc(size_t size);
void Free(void* ptr);

void* LoadPeImage(void* peBuffer);
void* GetExportByName(void* imageBase, const char* functionName);

#define DECL_FN(ret, name, args, ptr) typedef ret (__cdecl *name##_t) args; extern name##_t p##name = NULL;

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

#undef DECL_FN