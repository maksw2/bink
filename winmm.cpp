#include <efi.h>
#include <efilib.h>
#include <intrin.h>
#include "print.h"

typedef unsigned long DWORD;
typedef int BOOL;

typedef DWORD (*timeGetTime_t)();
timeGetTime_t ptimeGetTime = nullptr;

DWORD timeGetTime() {
    EFI_TIME t;
    if (EFI_SUCCESS == gRT->GetTime(&t, nullptr)) {
        // convert to milliseconds since midnight
        return (DWORD)(
            ((t.Hour * 3600 + t.Minute * 60 + t.Second) * 1000ULL) +
            (t.Nanosecond / 1000000ULL)
        );
    }
    return 0;
}

//
// HDA I/O protocol.
//
 
//
// HDA I/O protocol GUID.
//
#define EFI_HDA_IO_PROTOCOL_GUID \
  { 0xA090D7F9, 0xB50A, 0x4EA1,  \
    { 0xBD, 0xE9, 0x1A, 0xA5, 0xE9, 0x81, 0x2F, 0x45 } }
 
typedef struct EFI_HDA_IO_PROTOCOL_ EFI_HDA_IO_PROTOCOL;
 
typedef enum {
  EfiHdaIoTypeInput,
  EfiHdaIoTypeOutput,
  EfiHdaIoTypeMaximum
} EFI_HDA_IO_PROTOCOL_TYPE;
 
typedef struct {
  UINT32    Count;
  UINT32    *Verbs;
  UINT32    *Responses;
} EFI_HDA_IO_VERB_LIST;
 
typedef
VOID
(EFIAPI *EFI_HDA_IO_STREAM_CALLBACK)(
  IN EFI_HDA_IO_PROTOCOL_TYPE   Type,
  IN VOID                       *Context1,
  IN VOID                       *Context2,
  IN VOID                       *Context3
  );
 
typedef
EFI_STATUS
(EFIAPI *EFI_HDA_IO_GET_ADDRESS)(
  IN  EFI_HDA_IO_PROTOCOL       *This,
  OUT UINT8                     *CodecAddress
  );
 
typedef
EFI_STATUS
(EFIAPI *EFI_HDA_IO_SEND_COMMAND)(
  IN  EFI_HDA_IO_PROTOCOL       *This,
  IN  UINT8                     Node,
  IN  UINT32                    Verb,
  OUT UINT32                    *Response
  );
 
typedef
EFI_STATUS
(EFIAPI *EFI_HDA_IO_SEND_COMMANDS)(
  IN     EFI_HDA_IO_PROTOCOL    *This,
  IN     UINT8                  Node,
  IN OUT EFI_HDA_IO_VERB_LIST   *Verbs
  );
 
typedef
EFI_STATUS
(EFIAPI *EFI_HDA_IO_SETUP_STREAM)(
  IN  EFI_HDA_IO_PROTOCOL       *This,
  IN  EFI_HDA_IO_PROTOCOL_TYPE  Type,
  IN  UINT16                    Format,
  OUT UINT8                     *StreamId
  );
 
typedef
EFI_STATUS
(EFIAPI *EFI_HDA_IO_CLOSE_STREAM)(
  IN EFI_HDA_IO_PROTOCOL         *This,
  IN EFI_HDA_IO_PROTOCOL_TYPE    Type
  );
 
typedef
EFI_STATUS
(EFIAPI *EFI_HDA_IO_GET_STREAM)(
  IN  EFI_HDA_IO_PROTOCOL        *This,
  IN  EFI_HDA_IO_PROTOCOL_TYPE   Type,
  OUT BOOLEAN                    *State
  );
 
typedef
EFI_STATUS
(EFIAPI *EFI_HDA_IO_START_STREAM)(
  IN EFI_HDA_IO_PROTOCOL         *This,
  IN EFI_HDA_IO_PROTOCOL_TYPE    Type,
  IN VOID                        *Buffer,
  IN UINTN                       BufferLength,
  IN UINTN                       BufferPosition  OPTIONAL,
  IN EFI_HDA_IO_STREAM_CALLBACK  Callback        OPTIONAL,
  IN VOID                        *Context1       OPTIONAL,
  IN VOID                        *Context2       OPTIONAL,
  IN VOID                        *Context3       OPTIONAL
  );
 
typedef
EFI_STATUS
(EFIAPI *EFI_HDA_IO_STOP_STREAM)(
  IN EFI_HDA_IO_PROTOCOL         *This,
  IN EFI_HDA_IO_PROTOCOL_TYPE    Type
  );
 
struct EFI_HDA_IO_PROTOCOL_ {
  EFI_HDA_IO_GET_ADDRESS      GetAddress;
  EFI_HDA_IO_SEND_COMMAND     SendCommand;
  EFI_HDA_IO_SEND_COMMANDS    SendCommands;
  EFI_HDA_IO_SETUP_STREAM     SetupStream;
  EFI_HDA_IO_CLOSE_STREAM     CloseStream;
  EFI_HDA_IO_GET_STREAM       GetStream;
  EFI_HDA_IO_START_STREAM     StartStream;
  EFI_HDA_IO_STOP_STREAM      StopStream;
};

EFI_GUID gEfiHdaIoProtocolGuid = EFI_HDA_IO_PROTOCOL_GUID;

//
// HDA I/O Device Path protocol.
//
 
#define EFI_HDA_IO_DEVICE_PATH_GUID \
  { 0xA9003FEB, 0xD806, 0x41DB,     \
    { 0xA4, 0x91, 0x54, 0x05, 0xFE, 0xEF, 0x46, 0xC3 } }
 
typedef struct {
  EFI_DEVICE_PATH_PROTOCOL    Header;
  EFI_GUID                    Guid;
  UINT32                      Address;
} EFI_HDA_IO_DEVICE_PATH;
 
static_assert (
  sizeof (EFI_HDA_IO_DEVICE_PATH)
  == sizeof (EFI_DEVICE_PATH_PROTOCOL) + sizeof (EFI_GUID) + sizeof (UINT32),
  "Unexpected EFI_HDA_IO_DEVICE_PATH size"
  );
 
EFI_GUID gEfiHdaIoDevicePathGuid = EFI_HDA_IO_DEVICE_PATH_GUID;
 
#define EFI_HDA_IO_DEVICE_PATH_TEMPLATE \
  { \
    { \
      MESSAGING_DEVICE_PATH, \
      MSG_VENDOR_DP, \
      { \
        (UINT8) (sizeof (EFI_HDA_IO_DEVICE_PATH) & 0xFFU), \
        (UINT8) ((sizeof (EFI_HDA_IO_DEVICE_PATH) >> 8U) & 0xFFU) \
      } \
    }, \
    gEfiHdaIoDevicePathGuid, \
    0 \
  }

#define VERB_SET_STREAM_FMT       0x20000
#define VERB_SET_AMP_GAIN         0x30000
#define VERB_SET_CONN_SELECT      0x70100
#define VERB_SET_PIN_CONTROL      0x70700
#define VERB_SET_EAPD_BTL         0x70C00
#define VERB_SET_CHAN_STREAMID    0x70600
#define PIN_OUT_ENABLE  0x40
#define AMP_OUT_UNMUTE_MAX  0xB07F
#define HDA_FMT_48K_16B_STEREO 0x0011 // 48kHz, 16-bit, Stereo

typedef union _LARGE_INTEGER {
    struct {
        unsigned long LowPart;
        long HighPart;
    };
    long long QuadPart;
} LARGE_INTEGER;

UINT32 waveOutOpen() { return 0; }
UINT32 waveOutWrite() { return 0; }
UINT32 waveOutPause() { return 0; }
UINT32 waveOutRestart() { return 0; }
UINT32 waveOutSetVolume() { return 0; }
UINT32 waveOutPrepareHeader() { return 0; }
UINT32 waveOutUnprepareHeader() { return 0; }
UINT32 waveOutGetPosition() { return 0; }
UINT32 waveOutReset() { return 0; }
UINT32 waveOutClose() { return 0; }

extern int stricmp(const char *s1, const char *s2);

void* ResolveWinMM(const char* functionName) {
    if (stricmp(functionName, "timeGetTime") == 0) {
        return timeGetTime;
    } else if (stricmp(functionName, "waveOutOpen") == 0) {
        return waveOutOpen;
    } else if (stricmp(functionName, "waveOutWrite") == 0) {
        return waveOutWrite;
    } else if (stricmp(functionName, "waveOutPause") == 0) {
        return waveOutPause;
    } else if (stricmp(functionName, "waveOutRestart") == 0) {
        return waveOutRestart;
    } else if (stricmp(functionName, "waveOutSetVolume") == 0) {
        return waveOutSetVolume;
    } else if (stricmp(functionName, "waveOutGetPosition") == 0) {
        return waveOutGetPosition;
    } else if (stricmp(functionName, "waveOutReset") == 0) {
        return waveOutReset;
    }
    
    printf("Unknown function: %s\n", functionName);
    return NULL;
}
