#define LOG_TAG "AudioWrapperCommon"
// #define LOG_NDEBUG 0

#include <cutils/log.h>
#include "common.h"

int load_vendor_module(const char * module_name, const hw_module_t ** vendor_module)
{
    int rv = 0;
    ALOGW("%s", __FUNCTION__);

    if(*vendor_module)
        return 0;

    rv = hw_get_module(module_name, vendor_module);
    if (rv)
        ALOGE("failed to open %s module", module_name);

    return rv;
}

audio_devices_t convert_ics_to_jb(ics_audio_devices_t ics_device) {

    audio_devices_t device = 0;

    if((ics_device & ~ICS_AUDIO_DEVICE_OUT_ALL) == 0) {
        /* The first 15 AUDIO_DEVICE_OUT bits are equal. Exception is
         * ICS_AUDIO_DEVICE_OUT_DEFAULT / AUDIO_DEVICE_OUT_REMOTE_SUBMIX.
         */
        device = ics_device & ~ICS_AUDIO_DEVICE_OUT_DEFAULT;

        /* Set the correct DEFAULT bit */
        if(ics_device & ICS_AUDIO_DEVICE_OUT_DEFAULT) {
            device |= AUDIO_DEVICE_OUT_DEFAULT;
        }
    } else if((ics_device & ~ICS_AUDIO_DEVICE_IN_ALL) == 0) {
        /* Bits needs to be shifted 16 bits to the right and the IN bit must be
         * set.
         */
        device = ((ics_device & ~ICS_AUDIO_DEVICE_IN_DEFAULT) >> 16) | AUDIO_DEVICE_BIT_IN;

        /* Set the correct DEFAULT bit */
        if((ics_device & ICS_AUDIO_DEVICE_IN_DEFAULT) == ICS_AUDIO_DEVICE_IN_DEFAULT) {
            device |= AUDIO_DEVICE_IN_DEFAULT;
        }
    } else {
        /* I guess we should never land here */
        ALOGW("ics_audio_devices_t is neither input nor output: 0x%x", ics_device);
        device = ics_device;
    }


#ifndef WRAPPER_CONVERT
    ALOGW("%s (disabled): 0x%x -> 0x%x", __FUNCTION__, ics_device, device);
    return ics_device;
#else
    ALOGW("%s: 0x%x -> 0x%x", __FUNCTION__, ics_device, device);
    return device;
#endif
}

#define DEVICE_OUT_MASK 0x3FFF
#define DEVICE_IN_MASK 0xFF

ics_audio_devices_t convert_jb_to_ics(audio_devices_t device) {

    ics_audio_devices_t ics_device = 0;

    if(audio_is_output_devices(device)) {
        /* We care only about the first 15 bits since the others cannot be
         * mapped to the old enum.
         */
        ics_device = (device & DEVICE_OUT_MASK);
        /* Set the correct DEFAULT bit */
        if(device & AUDIO_DEVICE_OUT_DEFAULT)
            ics_device |= ICS_AUDIO_DEVICE_OUT_DEFAULT;
    } else if((device & AUDIO_DEVICE_BIT_IN) == AUDIO_DEVICE_BIT_IN) {
        /* We care only about the first 8 bits since the other cannot be
         * mapped to the old enum.
         */
        ics_device = (device & DEVICE_IN_MASK) << 16;
        if((device & AUDIO_DEVICE_IN_DEFAULT) == AUDIO_DEVICE_IN_DEFAULT)
            ics_device |= ICS_AUDIO_DEVICE_IN_DEFAULT;
    } else {
        /* I guess we should never land here */
        ALOGW("audio_devices_t is neither input nor output: 0x%x", device);
        ics_device = device;
    }

#ifndef WRAPPER_CONVERT
    ALOGW("%s (disabled): 0x%x -> 0x%x", __FUNCTION__, device, ics_device);
    return device;
#else
    ALOGW("%s: 0x%x -> 0x%x", __FUNCTION__, device, ics_device);
    return ics_device;
#endif
}

char * fixup_audio_parameters(const char *kv_pairs, bool jb_to_ics)
{
    int value;
    size_t len;
    char *out;

    android::AudioParameter param = android::AudioParameter(android::String8(kv_pairs));
    android::String8 key = android::String8(android::AudioParameter::keyRouting);
    if (param.getInt(key, value) == android::NO_ERROR) {
        ALOGW("%s: Fixing routing value (value: %u, jb_to_ics: %d)", __FUNCTION__,
              value, jb_to_ics);
        if(jb_to_ics) {
            value = convert_jb_to_ics(value);
        } else {
            value = convert_ics_to_jb(value);
        }

        /* Adds value as a singed int that might be negative. Doesn't cause any
         * problems because the bit representation is the same. */
        param.addInt(key, value);
        
        android::String8 fixed_kv_pairs = param.toString();
        len = fixed_kv_pairs.length();
        out = (char *) malloc(len + 1);
        strcpy(out, fixed_kv_pairs.string());

        ALOGW("%s: fixed_kv_pairs: %s (%d)", __FUNCTION__, out, strlen(out));
    } else {
        len = strlen(kv_pairs);
        out = (char *) malloc(len + 1);
        strcpy(out, kv_pairs);
    }

    return out;
}
