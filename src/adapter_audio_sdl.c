#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#if !defined(DISABLE_AUDIO)
#include "SDL_compat.h"
#include "../include/adapter_audio.h"
#include "runtime_loaders.h"
#include <math.h>

// PROV: DirectSound/WinMM API replacements based on runtime.apis.json analysis
// Evidence: DirectSoundCreate at IAT_0x4c4cd0, used for audio initialization
// Evidence: auxGetVolume/auxSetVolume for volume control
// Evidence: mciSendCommandA/mciSendStringA for video/music playback

static DSOUND_CONTEXT g_audio_context = {0};

/* [RESOURCE:AUDIO] DirectSoundCreate Adapter
 * PROV: va=0x401000+, iat=0x4c4cd0, src=runtime.apis.json
 * NOTE: Initialize DirectSound interface for audio playback
 * RESOURCE: 183 WAV sound effects (1KB-200KB) from Sdata/
 * ADAPTER: SDL2_mixer replacement for DirectSound
 */
HRESULT adapter_DirectSoundCreate(GUID *lpGuid, void **ppDS, IUnknown *pUnkOuter)
{
    if (!ppDS) return DSERR_INVALIDPARAM;
    
    // Initialize audio system if not already done
    if (adapter_audio_init() != 0) {
        return DSERR_GENERIC;
    }
    
    // Return context as DirectSound interface pointer
    *ppDS = &g_audio_context;
    g_audio_context.initialized = 1;
    
    return DS_OK;
}

// PROV: Secondary buffer creation for individual sound effects
// Resource evidence: 183 WAV files ranging from 1KB to 200KB
HRESULT adapter_CreateSoundBuffer(void *lpDSBufferDesc, DSOUND_BUFFER **ppDSBuffer, IUnknown *pUnkOuter)
{
    if (!ppDSBuffer) return DSERR_INVALIDPARAM;
    if (!g_audio_context.initialized) return DSERR_GENERIC;
    
    DSOUND_BUFFER *buffer = calloc(1, sizeof(DSOUND_BUFFER));
    if (!buffer) return DSERR_OUTOFMEMORY;
    
    // Initialize buffer with default settings based on WAV analysis
    // PROV: Evidence shows 16-bit mono PCM, 11025/22050 Hz sample rates
    buffer->frequency = ADAPTER_AUDIO_FREQUENCY;
    buffer->format = ADAPTER_AUDIO_FORMAT;  
    buffer->channels = ADAPTER_AUDIO_CHANNELS;
    buffer->channel = -1; // Unassigned
    
    *ppDSBuffer = buffer;
    return DS_OK;
}

// PROV: auxGetVolume call sites at VA_0x401000+, IAT_0x4c4d20
// Used for reading system volume levels
MMRESULT adapter_auxGetVolume(UINT_PTR uDeviceID, LPDWORD pdwVolume)
{
    if (!pdwVolume) return MMSYSERR_INVALHANDLE;
    
    // Return current Mix_VolumeMusic level scaled to Windows range (0-65535)
    int mix_volume = Mix_VolumeMusic(-1); // -1 = query current volume
    *pdwVolume = (mix_volume * ADAPTER_VOLUME_MAX) / MIX_MAX_VOLUME;
    
    return MMSYSERR_NOERROR;
}

// PROV: auxSetVolume call sites at VA_0x401000+, IAT_0x4c4d2c
// Used for setting system volume levels
MMRESULT adapter_auxSetVolume(UINT_PTR uDeviceID, DWORD dwVolume)
{
    // Scale Windows volume (0-65535) to Mix_Music range (0-MIX_MAX_VOLUME)
    int mix_volume = (dwVolume * MIX_MAX_VOLUME) / ADAPTER_VOLUME_MAX;
    Mix_VolumeMusic(mix_volume);
    
    // Also set chunk volume for sound effects
    Mix_Volume(-1, mix_volume); // -1 = all channels
    
    g_audio_context.volume = dwVolume;
    return MMSYSERR_NOERROR;
}

// PROV: auxGetDevCapsA call sites for audio device information
MMRESULT adapter_auxGetDevCapsA(UINT_PTR uDeviceID, LPVOID lpCaps, UINT cbCaps)
{
    if (!lpCaps || cbCaps == 0) return MMSYSERR_INVALHANDLE;
    
    // Return minimal capabilities - SDL_mixer handles device details
    memset(lpCaps, 0, cbCaps);
    return MMSYSERR_NOERROR;
}

// PROV: auxGetNumDevs call sites - return 1 for SDL_mixer default device
UINT adapter_auxGetNumDevs(void)
{
    return 1; // SDL_mixer provides unified audio interface
}

// PROV: mciSendCommandA call sites at VA_0x401000+, IAT_0x4c4d38
// Used for controlling media playback (17 .san movie files)
MCIERROR adapter_mciSendCommandA(MCIDEVICEID wDeviceID, UINT uMessage, DWORD_PTR fdwCommand, DWORD_PTR dwParam)
{
    // PROV: Evidence shows MCI usage for video playback control
    // For now, return success - full MCI implementation would require
    // additional evidence about specific command patterns used
    return MMSYSERR_NOERROR;
}

// PROV: mciSendStringA call sites at VA_0x401000+, IAT_0x4c4d44
// String-based media control for .san files
MCIERROR adapter_mciSendStringA(LPCSTR lpstrCommand, LPSTR lpstrReturnString, UINT uReturnLength, HWND hwndCallback)
{
    if (!lpstrCommand) return MMSYSERR_ERROR;
    
    // TODO_EVID: Need analysis of actual MCI command strings used
    // Common patterns: "open", "play", "stop", "close", "status"
    
    if (lpstrReturnString && uReturnLength > 0) {
        lpstrReturnString[0] = '\0';
    }
    
    return MMSYSERR_NOERROR;
}

// Sound buffer operations
HRESULT adapter_PlaySound(DSOUND_BUFFER *buffer, uint32_t flags)
{
    if (!buffer || !buffer->chunk) return DSERR_INVALIDPARAM;
    
    int loops = (flags & DSBPLAY_LOOPING) ? -1 : 0;
    buffer->channel = Mix_PlayChannel(-1, buffer->chunk, loops);
    
    if (buffer->channel == -1) {
        return DSERR_GENERIC;
    }
    
    buffer->playing = 1;
    return DS_OK;
}

HRESULT adapter_StopSound(DSOUND_BUFFER *buffer)
{
    if (!buffer) return DSERR_INVALIDPARAM;
    
    if (buffer->channel != -1) {
        Mix_HaltChannel(buffer->channel);
        buffer->channel = -1;
    }
    
    buffer->playing = 0;
    return DS_OK;
}

HRESULT adapter_SetVolume(DSOUND_BUFFER *buffer, int32_t volume)
{
    if (!buffer) return DSERR_INVALIDPARAM;
    
    // Convert DirectSound volume (dB * 100) to SDL_mixer volume (0-128)
    // DirectSound uses negative dB values, with 0 = max volume
    int mix_volume = MIX_MAX_VOLUME;
    if (volume < 0) {
        // Approximate dB to linear conversion
        mix_volume = MIX_MAX_VOLUME * pow(10.0, volume / 2000.0);
        if (mix_volume < 0) mix_volume = 0;
        if (mix_volume > MIX_MAX_VOLUME) mix_volume = MIX_MAX_VOLUME;
    }
    
    if (buffer->chunk) {
        Mix_VolumeChunk(buffer->chunk, mix_volume);
    }
    
    return DS_OK;
}

HRESULT adapter_SetFrequency(DSOUND_BUFFER *buffer, uint32_t frequency)
{
    if (!buffer) return DSERR_INVALIDPARAM;
    
    // SDL_mixer doesn't support runtime frequency changes on individual chunks
    // This would require resampling the audio data
    buffer->frequency = frequency;
    
    return DS_OK; // Return success but frequency change not implemented
}

// Buffer data management (for dynamic audio loading)
HRESULT adapter_Lock(DSOUND_BUFFER *buffer, uint32_t offset, uint32_t size, void **ptr1, uint32_t *bytes1, void **ptr2, uint32_t *bytes2, uint32_t flags)
{
    // SDL_mixer doesn't support direct buffer locking
    // This would be used for streaming audio or procedural generation
    return DSERR_GENERIC;
}

HRESULT adapter_Unlock(DSOUND_BUFFER *buffer, void *ptr1, uint32_t bytes1, void *ptr2, uint32_t bytes2)
{
    // Corresponding unlock for buffer locking
    return DSERR_GENERIC;
}

// PROV: Load WAV files using runtime_loaders.c integration
// Evidence: 183 WAV files, 16-bit mono PCM, various sample rates
DSOUND_BUFFER* adapter_LoadWAV(const char *filename)
{
    if (!filename) return NULL;
    
    // Use the runtime loader to parse WAV file
    WAV_SOUND *wav = wav_load(filename);
    if (!wav) return NULL;
    
    DSOUND_BUFFER *buffer = calloc(1, sizeof(DSOUND_BUFFER));
    if (!buffer) {
        wav_free(wav);
        return NULL;
    }
    
    // Create Mix_Chunk from WAV data
    void *rw = SDL_RWFromMem(wav->samples, wav->data_size);
    if (rw) {
        buffer->chunk = Mix_LoadWAV_RW(rw, 1); // 1 = free RW automatically
    }
    
    if (!buffer->chunk) {
        wav_free(wav);
        free(buffer);
        return NULL;
    }
    
    buffer->frequency = wav->sample_rate;
    buffer->format = (wav->bits_per_sample == 16) ? AUDIO_S16LSB : AUDIO_U8;
    buffer->channels = wav->channels;
    buffer->channel = -1;
    
    wav_free(wav);
    return buffer;
}

void adapter_FreeWAV(DSOUND_BUFFER *buffer)
{
    if (buffer) {
        if (buffer->playing && buffer->channel != -1) {
            Mix_HaltChannel(buffer->channel);
        }
        if (buffer->chunk) {
            Mix_FreeChunk(buffer->chunk);
        }
        free(buffer);
    }
}

// Context management
int adapter_audio_init(void)
{
    if (g_audio_context.initialized) return 0;
    
    // Initialize SDL audio subsystem
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        fprintf(stderr, "SDL_Init(AUDIO) failed: %s\n", SDL_GetError());
        return -1;
    }
    
    // PROV: Audio format based on WAV analysis - 22050 Hz, 16-bit, mono
    if (Mix_OpenAudio(ADAPTER_AUDIO_FREQUENCY, ADAPTER_AUDIO_FORMAT, ADAPTER_AUDIO_CHANNELS, ADAPTER_AUDIO_CHUNKSIZE) < 0) {
        fprintf(stderr, "Mix_OpenAudio failed: %s\n", Mix_GetError());
        SDL_Quit();
        return -1;
    }
    
    // Allocate channels for simultaneous sound effects
    // PROV: Game likely plays multiple sounds simultaneously
    Mix_AllocateChannels(16);
    
    g_audio_context.initialized = 1;
    g_audio_context.volume = ADAPTER_VOLUME_MAX;
    
    return 0;
}

void adapter_audio_shutdown(void)
{
    if (g_audio_context.sound_chunks) {
        for (uint32_t i = 0; i < g_audio_context.num_chunks; i++) {
            if (g_audio_context.sound_chunks[i]) {
                Mix_FreeChunk(g_audio_context.sound_chunks[i]);
            }
        }
        free(g_audio_context.sound_chunks);
    }
    
    if (g_audio_context.music_tracks) {
        for (uint32_t i = 0; i < g_audio_context.num_music; i++) {
            if (g_audio_context.music_tracks[i]) {
                Mix_FreeMusic(g_audio_context.music_tracks[i]);
            }
        }
        free(g_audio_context.music_tracks);
    }
    
    Mix_CloseAudio();
    SDL_Quit();
    
    memset(&g_audio_context, 0, sizeof(g_audio_context));
}

DSOUND_CONTEXT* adapter_get_audio_context(void)
{
    return &g_audio_context;
}

// PROV: SDL main loop audio functions (optional, with HAVE_ADAPTER_AUDIO guards)
// Optional audio initialization for main loop
int adapter_audio_sdl_init(void)
{
#ifdef HAVE_ADAPTER_AUDIO
    // PROV: Main loop audio init - optional for disc-less mode
    // Returns 0 on success, negative on failure (no-audio is acceptable)
    int result = adapter_audio_init();
    if (result != 0) {
        // TODO_EVID: Log audio init failure but continue without audio
        fprintf(stderr, "[AUDIO] SDL audio init failed, continuing without audio\n");
        return -1;  // Non-fatal failure
    }
    return 0;  // Success
#else
    // TODO_EVID: Audio disabled at compile time
    fprintf(stderr, "[AUDIO] Audio support disabled at compile time\n");
    return -1;  // No audio support
#endif
}

// Optional audio shutdown for main loop  
void adapter_audio_sdl_shutdown(void)
{
#ifdef HAVE_ADAPTER_AUDIO
    // PROV: Main loop audio shutdown - clean up resources
    adapter_audio_shutdown();
#endif
    // No-op if audio not enabled
}

#else // DISABLE_AUDIO

/* Audio stubs when DISABLE_AUDIO is enabled
 * Provides all expected audio functions but with no-op implementations
 * Allows building without SDL2_mixer dependency
 */

// Define missing types when audio is disabled
typedef long HRESULT;
typedef unsigned long UINT_PTR;
typedef unsigned long *LPDWORD;
typedef unsigned long DWORD;
typedef unsigned int UINT;
typedef void *LPVOID;
typedef const char *LPCSTR;
typedef char *LPSTR;
typedef void *HWND;
typedef unsigned long MCIDEVICEID;
typedef unsigned long long DWORD_PTR;
typedef long MMRESULT;
typedef unsigned long MCIERROR;

// Dummy context for disabled audio mode
static int g_audio_disabled = 1;

/* [STUB] DirectSoundCreate - No Audio Mode
 * Returns DS_OK but doesn't initialize any audio subsystem
 */
HRESULT adapter_DirectSoundCreate(void *lpGuid, void **ppDS, void *pUnkOuter)
{
    (void)lpGuid; (void)pUnkOuter;
    if (!ppDS) return -1; // DSERR_INVALIDPARAM equivalent

    // Return dummy pointer - application won't use DirectSound features
    *ppDS = &g_audio_disabled;
    printf("[AUDIO] DirectSoundCreate - DISABLED (compiled with DISABLE_AUDIO=1)\n");
    return 0; // DS_OK equivalent
}

/* [STUB] CreateSoundBuffer - No Audio Mode */
HRESULT adapter_CreateSoundBuffer(void *lpDSBufferDesc, void **ppDSBuffer, void *pUnkOuter)
{
    (void)lpDSBufferDesc; (void)pUnkOuter;
    if (!ppDSBuffer) return -1;

    // Return dummy pointer for no-op buffer
    *ppDSBuffer = &g_audio_disabled;
    return 0;
}

/* [STUB] auxGetVolume - No Audio Mode */
MMRESULT adapter_auxGetVolume(UINT_PTR uDeviceID, LPDWORD pdwVolume)
{
    (void)uDeviceID;
    if (!pdwVolume) return -1;

    // Return max volume (65535) to indicate "audio working"
    *pdwVolume = 65535;
    return 0; // MMSYSERR_NOERROR equivalent
}

/* [STUB] auxSetVolume - No Audio Mode */
MMRESULT adapter_auxSetVolume(UINT_PTR uDeviceID, DWORD dwVolume)
{
    (void)uDeviceID; (void)dwVolume;
    return 0; // Success but no-op
}

/* [STUB] auxGetDevCapsA - No Audio Mode */
MMRESULT adapter_auxGetDevCapsA(UINT_PTR uDeviceID, LPVOID lpCaps, UINT cbCaps)
{
    (void)uDeviceID;
    if (!lpCaps || cbCaps == 0) return -1;

    // Zero out caps structure
    memset(lpCaps, 0, cbCaps);
    return 0;
}

/* [STUB] auxGetNumDevs - No Audio Mode */
UINT adapter_auxGetNumDevs(void)
{
    return 1; // Return 1 device to avoid "no audio" error paths
}

/* [STUB] mciSendCommandA - No Audio Mode */
MCIERROR adapter_mciSendCommandA(MCIDEVICEID wDeviceID, UINT uMessage, DWORD_PTR fdwCommand, DWORD_PTR dwParam)
{
    (void)wDeviceID; (void)uMessage; (void)fdwCommand; (void)dwParam;
    return 0; // Success but no-op
}

/* [STUB] mciSendStringA - No Audio Mode */
MCIERROR adapter_mciSendStringA(LPCSTR lpstrCommand, LPSTR lpstrReturnString, UINT uReturnLength, HWND hwndCallback)
{
    (void)lpstrCommand; (void)hwndCallback;
    if (lpstrReturnString && uReturnLength > 0) {
        lpstrReturnString[0] = '\0';
    }
    return 0;
}

/* [STUB] Sound buffer operations - No Audio Mode */
HRESULT adapter_PlaySound(void *buffer, uint32_t flags)
{
    (void)buffer; (void)flags;
    return 0; // Success but no-op
}

HRESULT adapter_StopSound(void *buffer)
{
    (void)buffer;
    return 0; // Success but no-op
}

HRESULT adapter_SetVolume(void *buffer, int32_t volume)
{
    (void)buffer; (void)volume;
    return 0; // Success but no-op
}

HRESULT adapter_SetFrequency(void *buffer, uint32_t frequency)
{
    (void)buffer; (void)frequency;
    return 0; // Success but no-op
}

HRESULT adapter_Lock(void *buffer, uint32_t offset, uint32_t size, void **ptr1, uint32_t *bytes1, void **ptr2, uint32_t *bytes2, uint32_t flags)
{
    (void)buffer; (void)offset; (void)size; (void)ptr1; (void)bytes1; (void)ptr2; (void)bytes2; (void)flags;
    return -1; // Not supported in no-audio mode
}

HRESULT adapter_Unlock(void *buffer, void *ptr1, uint32_t bytes1, void *ptr2, uint32_t bytes2)
{
    (void)buffer; (void)ptr1; (void)bytes1; (void)ptr2; (void)bytes2;
    return -1; // Not supported in no-audio mode
}

/* [STUB] WAV Loading - No Audio Mode */
void* adapter_LoadWAV(const char *filename)
{
    (void)filename;
    printf("[AUDIO] LoadWAV('%s') - DISABLED (compiled with DISABLE_AUDIO=1)\n", filename ? filename : "NULL");
    return &g_audio_disabled; // Return dummy pointer
}

void adapter_FreeWAV(void *buffer)
{
    (void)buffer;
    // No-op for disabled audio
}

/* [STUB] Context management - No Audio Mode */
int adapter_audio_init(void)
{
    printf("[AUDIO] Audio initialization DISABLED (compiled with DISABLE_AUDIO=1)\n");
    return 0; // Success - no actual initialization needed
}

void adapter_audio_shutdown(void)
{
    // No-op for disabled audio
}

void* adapter_get_audio_context(void)
{
    return &g_audio_disabled;
}

/* [STUB] SDL main loop audio functions - No Audio Mode */
int adapter_audio_sdl_init(void)
{
    printf("[AUDIO] SDL audio init DISABLED (compiled with DISABLE_AUDIO=1)\n");
    return -1; // Indicate no audio available (non-fatal)
}

void adapter_audio_sdl_shutdown(void)
{
    // No-op for disabled audio
}

#endif // DISABLE_AUDIO