/*
 * Copyright (C) 2013 Thomas Wendt <thoemy@gmx.net>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef AUDIO_WRAPPER_COMMON_H
#define AUDIO_WRAPPER_COMMON_H

#include <media/AudioParameter.h>
#include <hardware/audio.h>
#include <hardware/hardware.h>
#include <system/audio.h>

enum flags_conversion_mode {
    ICS_TO_JB,
    JB_TO_ICS,
};
typedef enum flags_conversion_mode flags_conversion_mode_t;

int load_vendor_module(const char * module_name, const hw_module_t ** vendor_module);
char * fixup_audio_parameters(const char *kv_pairs, flags_conversion_mode_t mode);

enum {
    /* output devices */
    ICS_AUDIO_DEVICE_OUT_EARPIECE                  = 0x1,
    ICS_AUDIO_DEVICE_OUT_SPEAKER                   = 0x2,
    ICS_AUDIO_DEVICE_OUT_WIRED_HEADSET             = 0x4,
    ICS_AUDIO_DEVICE_OUT_WIRED_HEADPHONE           = 0x8,
    ICS_AUDIO_DEVICE_OUT_BLUETOOTH_SCO             = 0x10,
    ICS_AUDIO_DEVICE_OUT_BLUETOOTH_SCO_HEADSET     = 0x20,
    ICS_AUDIO_DEVICE_OUT_BLUETOOTH_SCO_CARKIT      = 0x40,
    ICS_AUDIO_DEVICE_OUT_BLUETOOTH_A2DP            = 0x80,
    ICS_AUDIO_DEVICE_OUT_BLUETOOTH_A2DP_HEADPHONES = 0x100,
    ICS_AUDIO_DEVICE_OUT_BLUETOOTH_A2DP_SPEAKER    = 0x200,
    ICS_AUDIO_DEVICE_OUT_AUX_DIGITAL               = 0x400,
    ICS_AUDIO_DEVICE_OUT_ANLG_DOCK_HEADSET         = 0x800,
    ICS_AUDIO_DEVICE_OUT_DGTL_DOCK_HEADSET         = 0x1000,
    ICS_AUDIO_DEVICE_OUT_USB_ACCESSORY             = 0x2000,
    ICS_AUDIO_DEVICE_OUT_USB_DEVICE                = 0x4000,
    ICS_AUDIO_DEVICE_OUT_DEFAULT                   = 0x8000,
    ICS_AUDIO_DEVICE_OUT_ALL      = (ICS_AUDIO_DEVICE_OUT_EARPIECE |
                                 ICS_AUDIO_DEVICE_OUT_SPEAKER |
                                 ICS_AUDIO_DEVICE_OUT_WIRED_HEADSET |
                                 ICS_AUDIO_DEVICE_OUT_WIRED_HEADPHONE |
                                 ICS_AUDIO_DEVICE_OUT_BLUETOOTH_SCO |
                                 ICS_AUDIO_DEVICE_OUT_BLUETOOTH_SCO_HEADSET |
                                 ICS_AUDIO_DEVICE_OUT_BLUETOOTH_SCO_CARKIT |
                                 ICS_AUDIO_DEVICE_OUT_BLUETOOTH_A2DP |
                                 ICS_AUDIO_DEVICE_OUT_BLUETOOTH_A2DP_HEADPHONES |
                                 ICS_AUDIO_DEVICE_OUT_BLUETOOTH_A2DP_SPEAKER |
                                 ICS_AUDIO_DEVICE_OUT_AUX_DIGITAL |
                                 ICS_AUDIO_DEVICE_OUT_ANLG_DOCK_HEADSET |
                                 ICS_AUDIO_DEVICE_OUT_DGTL_DOCK_HEADSET |
                                 ICS_AUDIO_DEVICE_OUT_USB_ACCESSORY |
                                 ICS_AUDIO_DEVICE_OUT_USB_DEVICE |
                                 ICS_AUDIO_DEVICE_OUT_DEFAULT),
    ICS_AUDIO_DEVICE_OUT_ALL_A2DP = (ICS_AUDIO_DEVICE_OUT_BLUETOOTH_A2DP |
                                 ICS_AUDIO_DEVICE_OUT_BLUETOOTH_A2DP_HEADPHONES |
                                 ICS_AUDIO_DEVICE_OUT_BLUETOOTH_A2DP_SPEAKER),
    ICS_AUDIO_DEVICE_OUT_ALL_SCO  = (ICS_AUDIO_DEVICE_OUT_BLUETOOTH_SCO |
                                 ICS_AUDIO_DEVICE_OUT_BLUETOOTH_SCO_HEADSET |
                                 ICS_AUDIO_DEVICE_OUT_BLUETOOTH_SCO_CARKIT),
    ICS_AUDIO_DEVICE_OUT_ALL_USB  = (ICS_AUDIO_DEVICE_OUT_USB_ACCESSORY |
                                 ICS_AUDIO_DEVICE_OUT_USB_DEVICE),

    /* input devices */
    ICS_AUDIO_DEVICE_IN_COMMUNICATION         = 0x10000,
    ICS_AUDIO_DEVICE_IN_AMBIENT               = 0x20000,
    ICS_AUDIO_DEVICE_IN_BUILTIN_MIC           = 0x40000,
    ICS_AUDIO_DEVICE_IN_BLUETOOTH_SCO_HEADSET = 0x80000,
    ICS_AUDIO_DEVICE_IN_WIRED_HEADSET         = 0x100000,
    ICS_AUDIO_DEVICE_IN_AUX_DIGITAL           = 0x200000,
    ICS_AUDIO_DEVICE_IN_VOICE_CALL            = 0x400000,
    ICS_AUDIO_DEVICE_IN_BACK_MIC              = 0x800000,
    ICS_AUDIO_DEVICE_IN_DEFAULT               = 0x80000000,

    ICS_AUDIO_DEVICE_IN_ALL     = (ICS_AUDIO_DEVICE_IN_COMMUNICATION |
                               ICS_AUDIO_DEVICE_IN_AMBIENT |
                               ICS_AUDIO_DEVICE_IN_BUILTIN_MIC |
                               ICS_AUDIO_DEVICE_IN_BLUETOOTH_SCO_HEADSET |
                               ICS_AUDIO_DEVICE_IN_WIRED_HEADSET |
                               ICS_AUDIO_DEVICE_IN_AUX_DIGITAL |
                               ICS_AUDIO_DEVICE_IN_VOICE_CALL |
                               ICS_AUDIO_DEVICE_IN_BACK_MIC |
                               ICS_AUDIO_DEVICE_IN_DEFAULT),
    ICS_AUDIO_DEVICE_IN_ALL_SCO = ICS_AUDIO_DEVICE_IN_BLUETOOTH_SCO_HEADSET,
};

typedef uint32_t ics_audio_devices_t;

audio_devices_t convert_ics_to_jb(ics_audio_devices_t ics_device);
ics_audio_devices_t convert_jb_to_ics(audio_devices_t device);


#endif // AUDIO_WRAPPER_COMMON_H
