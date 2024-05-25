/* Copyright (C) 2024 The uOFW team
   See the file COPYING for copying permission.
*/

#include <audio.h>
#include <common_imp.h>
#include <modulemgr_init.h>
#include <sysmem_user.h>
#include <threadman_kernel.h>
#include "vaudio.h"

SCE_MODULE_INFO("sceVaudio_driver", SCE_MODULE_KERNEL | SCE_MODULE_ATTR_EXCLUSIVE_START | SCE_MODULE_ATTR_EXCLUSIVE_LOAD, 1, 8);
SCE_MODULE_BOOTSTART("sceVaudioInit");
SCE_MODULE_STOP("sceVaudioEnd");
SCE_SDK_VERSION(SDK_VERSION);

// Imports
extern s32 sceMeCore_driver_635397BB(s32, s32 sampleCount, s32 channel);

// Globals
u8 g_unk0; // 0x0000145E
u8 g_unk1; // 0x0000145F
u8 g_type; // 0x00001460
u8 g_mode; // 0x00001463
s32 g_unk2; // 0x00001466
SceUID g_vaudio_sema; // 0x00001454
SceUID g_vaudio_fpl; // 0x00001450
u32 g_data; // 0x00001458

// Subroutine sub_000004D0 - Address 0x000004D0
s32 sub_000004D0(void)
{
    u32 *data = NULL;
    g_vaudio_sema = sceKernelCreateSema("SceVaudio", 0x101, 1, 1, 0);
    s32 ret = g_vaudio_sema;
    
    if (g_vaudio_sema >= 0)
    {
        g_vaudio_fpl = sceKernelCreateFpl("SceVaudioWork", SCE_KERNEL_PRIMARY_USER_PARTITION, 0x100, 0xa300, 1, 0);
        
        if (g_vaudio_fpl < 0)
        {
            sceKernelDeleteSema(g_vaudio_sema);
            ret = g_vaudio_fpl;
        }
        else
        {
            ret = sceKernelAllocateFpl(g_vaudio_fpl, (void **)&data, 0);
            if (ret < 0)
            {
                sceKernelDeleteSema(g_vaudio_sema);
                sceKernelDeleteFpl(g_vaudio_fpl);
            }
            else
            {
                g_data = data[0] | (((s32)data[0] >> 0x1f) + 2) * 0x20000000;
                ret = 0;
            }
        }
    }
    
    return ret;
}

s32 sub_00000AC8(s32 channel, s32 sampleCount, s32 format, s32 arg3, s32 arg4)
{
    (void)channel;
    (void)sampleCount;
    (void)format;
    (void)arg3;
    (void)arg4;
    return 0;
}

s32 sub_00000D84(s32 freq)
{
    switch (freq) {
        case 8000:
            return 8;
        case 11025:
            return 6;
        case 12000:
            return 7;
        case 16000:
            return 5;
        case 22050:
            return 3;
        case 24000:
            return 4;
        case 32000:
            return 2;
        case 44100:
            return 0;
        case 48000:
            return 1;
    }

    return 0;
}

// Subroutine module_start - Address 0x00000670
s32 sceVaudioInit(SceSize args, const void *argp)
{
    (void)args;
    (void)argp;
    return (sub_000004D0() < 0);
}

// Subroutine module_stop - Address 0x00000690
s32 sceVaudioEnd(SceSize args, const void *argp)
{
    (void)args;
    (void)argp;
    
    if (g_unk1 != '\0')
    {
        sceVaudioChRelease();
    }
    
    sceKernelDeleteSema(g_vaudio_sema);
    sceKernelDeleteFpl(g_vaudio_fpl);
    return 0;
}

// Subroutine sceVaudio_driver_03B6807D - Address 0x000005BC - Aliases: sceVaudioChReserve
s32 sceVaudioChReserve(s32 channel, s32 sampleCount, s32 format)
{
    return sub_00000AC8(channel, sampleCount, format, 0, -1);
}

// Subroutine sceVaudio_driver_27ACC20B - Address 0x000005DC - Aliases: sceVaudioChReserveBuffering
s32 sceVaudioChReserveBuffering(s32 channel, s32 sampleCount, s32 format)
{
    return sub_00000AC8(channel, sampleCount, format, 1, -1);
}

// Subroutine sceVaudio_driver_346FBE94 - Address 0x00000638 - Aliases: sceVaudioSetEffectType
s32 sceVaudioSetEffectType(s32 type)
{
    return sceVaudio_driver_A3B71098(type, 0);
}

// Subroutine sceVaudio_driver_67585DFD - Address 0x00000264 - Aliases: sceVaudioChRelease
s32 sceVaudioChRelease(void)
{
    s32 ret = 0;
    ret = sceKernelWaitSema(g_vaudio_sema, 1, 0);

    if (ret >= 0)
    {
        sceAudioSRCOutputBlocking(0, 0);
        ret = sceAudioSRCChRelease();
        
        if (ret >= 0)
        {
            if (g_unk2 > 0)
            {
                g_unk2 = -1;
                sceMeCore_driver_635397BB(0x126, 0, 0);
            }
            
            sceAudioSetFrequency(44100);

            if (g_unk0 == '\x02')
            {
                g_unk0 = '\0';
                s32 appType = sceKernelApplicationType();

                if (appType == SCE_INIT_APPLICATION_VSH)
                {
                    sceAudioSetVolumeOffset(0);
                }
            }
            
            g_unk1 = 0;
        }
        
        sceKernelSignalSema(g_vaudio_sema, 1);
    }
    
    return ret;
}

// Subroutine sceVaudio_driver_8986295E - Address 0x00000000 - Aliases: sceVaudioOutputBlocking
s32 sceVaudioOutputBlocking(void)
{
    return 0;
}

// Subroutine sceVaudio_driver_A3B71098 - Address 0x00000350
s32 sceVaudio_driver_A3B71098(s32 type, s32 arg)
{
    s32 ret = SCE_ERROR_INVALID_INDEX;

    if (type < SCE_VAUDIO_TYPE_MAX)
    {
        ret = sceKernelWaitSema(g_vaudio_sema, 1, 0);
        if (ret >= 0)
        {
            if ((s32)g_type != type)
            {
                g_type = (u8)type;
                g_unk0 = 1;
                
                s32 samples = sceAudioOutput2GetRestSample();
                if (samples < 1)
                {
                    s32 offset = g_type != '\0';
                    s32 appType = sceKernelApplicationType();
                    
                    if (appType == SCE_INIT_APPLICATION_VSH)
                    {
                        sceAudioSetVolumeOffset(offset);
                    }
                }
            }
            
            sceKernelSignalSema(g_vaudio_sema, 1);
            ret = arg;
        }
    }
    
    return ret;
}

// Subroutine sceVaudio_driver_CBD4AC51 - Address 0x00000434 - Aliases: sceVaudioSetAlcMode
s32 sceVaudioSetAlcMode(s32 mode)
{
    s32 ret = SCE_ERROR_INVALID_INDEX;

    if (mode < SCE_VAUDIO_ALC_MODE_MAX)
    {
        ret = sceKernelWaitSema(g_vaudio_sema, 1, 0);
        if (ret >= 0)
        {
            if ((s32)g_mode != mode)
            {
                g_mode = (u8)mode;
                g_unk0 = 1;
            }
            
            sceKernelSignalSema(g_vaudio_sema, 1);
            ret = 0;
        }
    }
    
    return ret;
}

// Subroutine sceVaudio_driver_DCC18F25 - Address 0x00000668
s32 sceVaudio_driver_DCC18F25(void)
{
    return SCE_VAUDIO_TYPE_MAX;
}

// Subroutine sceVaudio_504E4745 - Address 0x000005FC
s32 sceVaudio_504E4745(s32 arg0)
{
    // SCE_ERROR_NOT_INITIALIZED
    s32 ret = 0x80000001;

    if (g_unk2 >= 0)
    {
        ret = SCE_ERROR_INVALID_VALUE;

        if (arg0 - 1024 < 0xc01)
        {
            g_unk2 = (short)arg0;
            ret = 0;
        }
    }
    
    return ret;
}

// Subroutine sceVaudio_E8E78DC8 - Address 0x0000019C
s32 sceVaudio_E8E78DC8(s32 channel, s32 sampleCount, s32 format)
{
    s32 ret = sceKernelWaitSema(g_vaudio_sema, 1, 0);
    
    if (ret >= 0)
    {
        ret = sceMeCore_driver_635397BB(0x124, sampleCount, channel);
        sceKernelSignalSema(g_vaudio_sema, 1);
        
        if (ret >= 0)
        {
            ret = sub_00000AC8(channel, sampleCount, format, 1, 0x800);
        }
    }

    return ret;
}
