/* Copyright (C) 2024 The uOFW team
   See the file COPYING for copying permission.
*/

// VAUDIO Types
#define SCE_VAUDIO_TYPE_MAX 5

// VAUDIO ALC modes
#define SCE_VAUDIO_ALC_MODE_OFF  0
#define SCE_VAUDIO_ALC_MODE_ON   1
#define SCE_VAUDIO_ALC_MODE_MAX  2

s32 sceVaudioChReserve(s32 channel, s32 sampleCount, s32 format);
s32 sceVaudioChReserveBuffering(s32 channel, s32 sampleCount, s32 format);
s32 sceVaudioSetEffectType(s32 type);
s32 sceVaudioChRelease(void);
s32 sceVaudioOutputBlocking(void);
s32 sceVaudioSetAlcMode(s32 mode);
s32 sceVaudio_driver_A3B71098(s32 type, s32 arg);
