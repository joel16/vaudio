#include <audio.h>
#include <common_imp.h>
#include <modulemgr_init.h>
#include <threadman_kernel.h>

SCE_MODULE_INFO("sceVaudio_driver", SCE_MODULE_KERNEL | SCE_MODULE_ATTR_EXCLUSIVE_START | SCE_MODULE_ATTR_EXCLUSIVE_LOAD, 1, 8);
SCE_MODULE_BOOTSTART("sceVaudioInit");
SCE_MODULE_STOP("sceVaudioEnd");
SCE_SDK_VERSION(SDK_VERSION);

// VAUDIO Types
#define SCE_VAUDIO_TYPE_MAX 5

// VAUDIO ALC modes
#define SCE_VAUDIO_ALC_MODE_OFF  0
#define SCE_VAUDIO_ALC_MODE_ON   1
#define SCE_VAUDIO_ALC_MODE_MAX  2

// Prototypes and aliases
s32 sceVaudioChReserve(void) __attribute__((alias("sceVaudio_driver_03B6807D")));
s32 sceVaudioChReserveBuffering(void) __attribute__((alias("sceVaudio_driver_27ACC20B")));
s32 sceVaudioSetEffectType(s32 type) __attribute__((alias("sceVaudio_driver_346FBE94")));
s32 sceVaudioChRelease(void) __attribute__((alias("sceVaudio_driver_67585DFD")));
s32 sceVaudioOutputBlocking(void) __attribute__((alias("sceVaudio_driver_8986295E")));
s32 sceVaudioSetAlcMode(s32 mode) __attribute__((alias("sceVaudio_driver_CBD4AC51")));
s32 sceVaudio_driver_A3B71098(s32 type, s32 arg);

// Imports
extern s32 sceMeCore_driver_635397BB(s32);

// Globals
u8 g_unk0; // 0x0000145E
u8 g_unk1; // 0x0000145F
u8 g_type; // 0x00001460
u8 g_mode; // 0x00001463
s32 g_unk2; // 0x00001466
SceUID g_vaudio_sema; // 0x00001454
SceUID g_vaudio_fpl; // 0x00001450

// Subroutine sub_000004D0 - Address 0x000004D0
s32 sub_000004D0(void)
{
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
s32 sceVaudio_driver_03B6807D(void)
{
    return 0;
}

// Subroutine sceVaudio_driver_27ACC20B - Address 0x000005DC - Aliases: sceVaudioChReserveBuffering
s32 sceVaudio_driver_27ACC20B(void)
{
    return 0;
}

// Subroutine sceVaudio_driver_346FBE94 - Address 0x00000638 - Aliases: sceVaudioSetEffectType
s32 sceVaudio_driver_346FBE94(s32 type)
{
    return sceVaudio_driver_A3B71098(type, 0);
}

// Subroutine sceVaudio_driver_67585DFD - Address 0x00000264 - Aliases: sceVaudioChRelease
s32 sceVaudio_driver_67585DFD(void)
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
                sceMeCore_driver_635397BB(0x126);
            }
            
            sceAudioSetFrequency(0xac44);

            if (g_unk0 == '\x02')
            {
                g_unk0 = '\0';
                
                s32 appType = sceKernelApplicationType();
                if (appType == 0x100)
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
s32 sceVaudio_driver_8986295E(void)
{
    return 0;
}

// Subroutine sceVaudio_driver_A3B71098 - Address 0x00000350
s32 sceVaudio_driver_A3B71098(s32 type, s32 arg)
{
    s32 ret = 0x80000102;

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
                    
                    if (appType == 0x100)
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
s32 sceVaudio_driver_CBD4AC51(s32 mode)
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
s32 sceVaudio_504E4745(void)
{
    return 0;
}

// Subroutine sceVaudio_E8E78DC8 - Address 0x0000019C
s32 sceVaudio_E8E78DC8(void)
{
    return 0;
}
