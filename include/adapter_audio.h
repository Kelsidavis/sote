#ifndef ADAPTER_AUDIO_H
#define ADAPTER_AUDIO_H

#if 0 /* PROV: SDL2 include disabled for RE-AGENT REBUILD m100 - using forward declarations */
#include <SDL2/SDL.h>
// SDL_mixer not available in basic SDL2 setup
typedef struct Mix_Chunk Mix_Chunk;
typedef struct Mix_Music Mix_Music;
#else
// PROV: Using real SDL2 headers, no stub declarations needed
// NOTE: All SDL types and functions provided by linked SDL2 library
typedef struct Mix_Chunk Mix_Chunk;
typedef struct Mix_Music Mix_Music;
#define AUDIO_S16LSB 0x8010
#define AUDIO_U8 0x0008
#define MIX_MAX_VOLUME 128
#endif
#include <stdint.h>
#include <windows.h>

// PROV: Evidence sources from runtime.apis.json
// DirectSoundCreate: VA_0x401000+, IAT_0x4c4cd0
// auxGetVolume/auxSetVolume: VA_0x401000+, IAT_0x4c4d20/0x4c4d2c
// mciSendCommandA/mciSendStringA: VA_0x401000+, IAT_0x4c4d38/0x4c4d44

// DirectSound interface simulation for SDL2_mixer
typedef struct {
    SDL_AudioSpec spec;
    SDL_AudioDeviceID device_id;
    Mix_Chunk **sound_chunks;
    Mix_Music **music_tracks;  
    uint32_t num_chunks;
    uint32_t num_music;
    uint32_t volume;
    uint32_t initialized;
} DSOUND_CONTEXT;

typedef struct {
    Mix_Chunk *chunk;
    int channel;
    uint32_t frequency;
    uint16_t format;
    uint8_t channels;
    uint32_t playing;
} DSOUND_BUFFER;

// DirectSound adapter functions
// PROV: DirectSoundCreate call sites at VA_0x401000+, IAT_0x4c4cd0
HRESULT adapter_DirectSoundCreate(GUID *lpGuid, void **ppDS, IUnknown *pUnkOuter);

// PROV: Secondary buffer creation for 183 WAV sound effects
HRESULT adapter_CreateSoundBuffer(void *lpDSBufferDesc, DSOUND_BUFFER **ppDSBuffer, IUnknown *pUnkOuter);

// Volume control
// PROV: auxGetVolume call sites at VA_0x401000+, IAT_0x4c4d20
MMRESULT adapter_auxGetVolume(UINT_PTR uDeviceID, LPDWORD pdwVolume);

// PROV: auxSetVolume call sites at VA_0x401000+, IAT_0x4c4d2c  
MMRESULT adapter_auxSetVolume(UINT_PTR uDeviceID, DWORD dwVolume);

// Device capabilities
// PROV: auxGetDevCapsA call sites for audio device enumeration
MMRESULT adapter_auxGetDevCapsA(UINT_PTR uDeviceID, LPVOID lpCaps, UINT cbCaps);

// PROV: auxGetNumDevs call sites for device count
UINT adapter_auxGetNumDevs(void);

// Media Control Interface (MCI) for video playback
// PROV: mciSendCommandA call sites at VA_0x401000+, IAT_0x4c4d38
MCIERROR adapter_mciSendCommandA(MCIDEVICEID wDeviceID, UINT uMessage, DWORD_PTR fdwCommand, DWORD_PTR dwParam);

// PROV: mciSendStringA call sites at VA_0x401000+, IAT_0x4c4d44 - for .san movie files
MCIERROR adapter_mciSendStringA(LPCSTR lpstrCommand, LPSTR lpstrReturnString, UINT uReturnLength, HWND hwndCallback);

// Buffer operations
HRESULT adapter_PlaySound(DSOUND_BUFFER *buffer, uint32_t flags);
HRESULT adapter_StopSound(DSOUND_BUFFER *buffer);
HRESULT adapter_SetVolume(DSOUND_BUFFER *buffer, int32_t volume);
HRESULT adapter_SetFrequency(DSOUND_BUFFER *buffer, uint32_t frequency);

// Buffer data management
HRESULT adapter_Lock(DSOUND_BUFFER *buffer, uint32_t offset, uint32_t size, void **ptr1, uint32_t *bytes1, void **ptr2, uint32_t *bytes2, uint32_t flags);
HRESULT adapter_Unlock(DSOUND_BUFFER *buffer, void *ptr1, uint32_t bytes1, void *ptr2, uint32_t bytes2);

// Utility functions
int adapter_audio_init(void);
void adapter_audio_shutdown(void);
DSOUND_CONTEXT* adapter_get_audio_context(void);

// PROV: SDL main loop audio functions (optional, with HAVE_ADAPTER_AUDIO guards)
int adapter_audio_sdl_init(void);     // PROV: Optional audio init, returns 0 or negative on no-audio
void adapter_audio_sdl_shutdown(void); // PROV: Optional audio shutdown

// Load WAV files using runtime loader integration
// PROV: 183 WAV files from resource.catalog.json, 16-bit mono PCM format
DSOUND_BUFFER* adapter_LoadWAV(const char *filename);
void adapter_FreeWAV(DSOUND_BUFFER *buffer);

// Constants from evidence  
// PROV: WAV analysis shows 11025 Hz and 22050 Hz sample rates
#define ADAPTER_AUDIO_FREQUENCY 22050
#define ADAPTER_AUDIO_FORMAT    AUDIO_S16LSB
#define ADAPTER_AUDIO_CHANNELS  1  // Mono from WAV analysis
#define ADAPTER_AUDIO_CHUNKSIZE 1024

// Volume control constants (0-65535 range for Windows compatibility)
#define ADAPTER_VOLUME_MAX      65535
#define ADAPTER_VOLUME_MIN      0

// DirectSound error codes
#define DS_OK                   0x00000000
#define DSERR_GENERIC           0x80004005
#define DSERR_INVALIDPARAM      0x80070057
#define DSERR_OUTOFMEMORY       0x8007000E
#define DSERR_NODRIVER          0x88781001
#define DSERR_ALREADYINITIALIZED 0x88781002

// MCI error codes
#define MMSYSERR_NOERROR        0
#define MMSYSERR_ERROR          1
#define MMSYSERR_BADDEVICEID    2
#define MMSYSERR_NOTENABLED     3
#define MMSYSERR_ALLOCATED      4
#define MMSYSERR_INVALHANDLE    5

// Play flags
#define DSBPLAY_LOOPING         0x00000001

#endif // ADAPTER_AUDIO_H