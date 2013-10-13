/*
 * Copyright (C) 2011 The Android Open Source Project
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

#define LOG_TAG "AudioPolicyWrapper"
//#define LOG_NDEBUG 0

#include <cutils/log.h>

#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <hardware/hardware.h>
#include <system/audio.h>
#include <system/audio_policy.h>
#include <hardware/audio_policy.h>

#include "ics_audio_policy.h"
#include "aps_wrapper.h"
#include "common.h"

static audio_policy_module_t *gVendorModule = 0;

struct wrapper_ap_module {
    struct audio_policy_module module;
};

struct wrapper_ap_device {
    struct audio_policy_device device;
    struct ics_audio_policy_device *vendor_device;
};

struct wrapper_audio_policy {
    struct audio_policy policy;

    struct audio_policy_service_ops *aps_ops;
    void *service;

    struct ics_audio_policy *vendor_policy;
    struct wrapped_aps * wrapped_aps;
};

#define DEVICE(d) ((struct wrapper_ap_device*) d)->device
#define VENDOR(d) ((struct wrapper_ap_device*) d)->vendor_device

/* Accesses the vendor policy from the audio policy struct */
#define VENDOR_POLICY(p) ((struct wrapper_audio_policy*) p)->vendor_policy


static int ap_set_device_connection_state(struct audio_policy *pol,
                                          audio_devices_t device,
                                          audio_policy_dev_state_t state,
                                          const char *device_address)
{
    ALOGW("%s", __FUNCTION__);
    device = convert_jb_to_ics(device);
    return VENDOR_POLICY(pol)->set_device_connection_state(VENDOR_POLICY(pol),
                                                           device, state, device_address);
}

static audio_policy_dev_state_t ap_get_device_connection_state(
                                            const struct audio_policy *pol,
                                            audio_devices_t device,
                                            const char *device_address)
{
    ALOGW("%s", __FUNCTION__);
    device = convert_jb_to_ics(device);
    return VENDOR_POLICY(pol)->get_device_connection_state(VENDOR_POLICY(pol),
                                                           device, device_address);
}

static void ap_set_phone_state(struct audio_policy *pol, audio_mode_t state)
{
    ALOGW("%s", __FUNCTION__);
    VENDOR_POLICY(pol)->set_phone_state(VENDOR_POLICY(pol), state);
}

// deprecated, never called
static void ap_set_ringer_mode(struct audio_policy *pol, uint32_t mode,
                               uint32_t mask)
{
    ALOGW("%s", __FUNCTION__);
    VENDOR_POLICY(pol)->set_ringer_mode(VENDOR_POLICY(pol), mode, mask);
}

static void ap_set_force_use(struct audio_policy *pol,
                          audio_policy_force_use_t usage,
                          audio_policy_forced_cfg_t config)
{
    ALOGW("%s", __FUNCTION__);
    VENDOR_POLICY(pol)->set_force_use(VENDOR_POLICY(pol), usage, config);
}

    /* retreive current device category forced for a given usage */
static audio_policy_forced_cfg_t ap_get_force_use(
                                               const struct audio_policy *pol,
                                               audio_policy_force_use_t usage)
{
    ALOGW("%s", __FUNCTION__);
    return VENDOR_POLICY(pol)->get_force_use(VENDOR_POLICY(pol), usage);
}

/* if can_mute is true, then audio streams that are marked ENFORCED_AUDIBLE
 * can still be muted. */
static void ap_set_can_mute_enforced_audible(struct audio_policy *pol,
                                             bool can_mute)
{
    ALOGW("%s", __FUNCTION__);
    VENDOR_POLICY(pol)->set_can_mute_enforced_audible(VENDOR_POLICY(pol), can_mute);
}

static int ap_init_check(const struct audio_policy *pol)
{
    return VENDOR_POLICY(pol)->init_check(VENDOR_POLICY(pol));
}

static audio_io_handle_t ap_get_output(struct audio_policy *pol,
                                       audio_stream_type_t stream,
                                       uint32_t sampling_rate,
                                       audio_format_t format,
                                       audio_channel_mask_t channelMask,
                                       audio_output_flags_t flags)
{
    ALOGW("%s", __FUNCTION__);
    return VENDOR_POLICY(pol)->get_output(VENDOR_POLICY(pol), stream, sampling_rate, format, channelMask, flags);
}

static int ap_start_output(struct audio_policy *pol, audio_io_handle_t output,
                           audio_stream_type_t stream, int session)
{
    ALOGW("%s", __FUNCTION__);
    return VENDOR_POLICY(pol)->start_output(VENDOR_POLICY(pol), output, stream, session);
}

static int ap_stop_output(struct audio_policy *pol, audio_io_handle_t output,
                          audio_stream_type_t stream, int session)
{
    ALOGW("%s", __FUNCTION__);
    return VENDOR_POLICY(pol)->stop_output(VENDOR_POLICY(pol), output, stream, session);
}

static void ap_release_output(struct audio_policy *pol,
                              audio_io_handle_t output)
{
    ALOGW("%s", __FUNCTION__);
    VENDOR_POLICY(pol)->release_output(VENDOR_POLICY(pol), output);
}

static audio_io_handle_t ap_get_input(struct audio_policy *pol, audio_source_t inputSource,
                                      uint32_t sampling_rate,
                                      audio_format_t format,
                                      audio_channel_mask_t channelMask,
                                      audio_in_acoustics_t acoustics)
{
    ALOGW("%s", __FUNCTION__);
    return VENDOR_POLICY(pol)->get_input(VENDOR_POLICY(pol), inputSource, sampling_rate, format, channelMask, acoustics);
}

static int ap_start_input(struct audio_policy *pol, audio_io_handle_t input)
{
    ALOGW("%s", __FUNCTION__);
    return VENDOR_POLICY(pol)->start_input(VENDOR_POLICY(pol), input);
}

static int ap_stop_input(struct audio_policy *pol, audio_io_handle_t input)
{
    ALOGW("%s", __FUNCTION__);
    return VENDOR_POLICY(pol)->stop_input(VENDOR_POLICY(pol), input);
}

static void ap_release_input(struct audio_policy *pol, audio_io_handle_t input)
{
    ALOGW("%s", __FUNCTION__);
    VENDOR_POLICY(pol)->release_input(VENDOR_POLICY(pol), input);
}

static void ap_init_stream_volume(struct audio_policy *pol,
                                  audio_stream_type_t stream, int index_min,
                                  int index_max)
{
    ALOGW("%s: stream %d, index_min %d, index_max: %d", __FUNCTION__, stream, index_min, index_max);
    VENDOR_POLICY(pol)->init_stream_volume(VENDOR_POLICY(pol), stream, index_min, index_max);
}

static int ap_set_stream_volume_index(struct audio_policy *pol,
                                      audio_stream_type_t stream,
                                      int index)
{
    ALOGW("%s: stream %d, index %d", __FUNCTION__, stream, index);
    return VENDOR_POLICY(pol)->set_stream_volume_index(VENDOR_POLICY(pol), stream, index);
}

static int ap_get_stream_volume_index(const struct audio_policy *pol,
                                      audio_stream_type_t stream,
                                      int *index)
{
    int ret = VENDOR_POLICY(pol)->get_stream_volume_index(VENDOR_POLICY(pol), stream, index);
    ALOGW("%s: stream %d, index %d", __FUNCTION__, stream, *index);
    return ret;
}

#ifndef ICS_AUDIO_BLOB
static int ap_set_stream_volume_index_for_device(struct audio_policy *pol,
                                      audio_stream_type_t stream,
                                      int index,
                                      audio_devices_t device)
{
    ALOGW("%s: stream %d, index %d, device: 0x%x", __FUNCTION__, stream, index, device);
    device = convert_jb_to_ics(device);
    //return VENDOR_POLICY(pol)->set_stream_volume_index_for_device(VENDOR_POLICY(pol), stream, index, device);
    return VENDOR_POLICY(pol)->set_stream_volume_index(VENDOR_POLICY(pol), stream, index);
}

static int ap_get_stream_volume_index_for_device(const struct audio_policy *pol,
                                      audio_stream_type_t stream,
                                      int *index,
                                      audio_devices_t device)
{
    int ret;
    device = convert_jb_to_ics(device);
    //ret = VENDOR_POLICY(pol)->get_stream_volume_index_for_device(VENDOR_POLICY(pol), stream, index, device);
    ret = VENDOR_POLICY(pol)->get_stream_volume_index(VENDOR_POLICY(pol), stream, index);
    ALOGW("%s: stream %d, index %d, device: 0x%x", __FUNCTION__, stream, *index, device);
    return ret;
}
#endif

static uint32_t ap_get_strategy_for_stream(const struct audio_policy *pol,
                                           audio_stream_type_t stream)
{
    ALOGW("%s", __FUNCTION__);
    return VENDOR_POLICY(pol)->get_strategy_for_stream(VENDOR_POLICY(pol), stream);
}

static audio_devices_t ap_get_devices_for_stream(const struct audio_policy *pol,
                                          audio_stream_type_t stream)
{
    ALOGW("%s", __FUNCTION__);
    return convert_ics_to_jb(VENDOR_POLICY(pol)->get_devices_for_stream(VENDOR_POLICY(pol), stream));
}

static audio_io_handle_t ap_get_output_for_effect(struct audio_policy *pol,
                                            const struct effect_descriptor_s *desc)
{
    ALOGW("%s", __FUNCTION__);
    return VENDOR_POLICY(pol)->get_output_for_effect(VENDOR_POLICY(pol), desc);
}

static int ap_register_effect(struct audio_policy *pol,
                              const struct effect_descriptor_s *desc,
                              audio_io_handle_t output,
                              uint32_t strategy,
                              int session,
                              int id)
{
    ALOGW("%s", __FUNCTION__);
    return VENDOR_POLICY(pol)->register_effect(VENDOR_POLICY(pol), desc, output, strategy, session, id);
}

static int ap_unregister_effect(struct audio_policy *pol, int id)
{
    ALOGW("%s", __FUNCTION__);
    return VENDOR_POLICY(pol)->unregister_effect(VENDOR_POLICY(pol), id);
}

static int ap_set_effect_enabled(struct audio_policy *pol, int id, bool enabled)
{
    ALOGW("%s", __FUNCTION__);
    return VENDOR_POLICY(pol)->set_effect_enabled(VENDOR_POLICY(pol), id, enabled);
}

static bool ap_is_stream_active(const struct audio_policy *pol, audio_stream_type_t stream,
                                uint32_t in_past_ms)
{
    ALOGW("%s", __FUNCTION__);
    return VENDOR_POLICY(pol)->is_stream_active(VENDOR_POLICY(pol), stream, in_past_ms);
}

static bool ap_is_stream_active_remotely(const struct audio_policy *pol, audio_stream_type_t stream,
                                uint32_t in_past_ms)
{
    ALOGW("%s", __FUNCTION__);
    return 0;
}

static int ap_dump(const struct audio_policy *pol, int fd)
{
    ALOGW("%s", __FUNCTION__);
    return VENDOR_POLICY(pol)->dump(VENDOR_POLICY(pol), fd);
}

static int create_wrapper_ap(const struct audio_policy_device *device,
                             struct audio_policy_service_ops *aps_ops,
                             void *service,
                             struct audio_policy **ap)
{
    struct wrapper_ap_device *dev;
    struct wrapper_audio_policy *dap;
    struct ics_audio_policy *iap;

    *ap = NULL;

    if (!service || !aps_ops)
        return -EINVAL;

    dap = (struct wrapper_audio_policy *)malloc(sizeof(*dap));
    if (!dap)
        return -ENOMEM;

    memset(dap, 0, sizeof(*dap));

    wrap_aps(service, aps_ops, &dap->wrapped_aps);


    dev = (struct wrapper_ap_device *)device;


    dev->vendor_device->create_audio_policy(dev->vendor_device,
                                            &dap->wrapped_aps->wrapped_aps_ops,
                                            dap->wrapped_aps, &iap);

    dap->vendor_policy = iap;

    dap->policy.set_device_connection_state = ap_set_device_connection_state;
    dap->policy.get_device_connection_state = ap_get_device_connection_state;
    dap->policy.set_phone_state = ap_set_phone_state;
    dap->policy.set_ringer_mode = ap_set_ringer_mode;
    dap->policy.set_force_use = ap_set_force_use;
    dap->policy.get_force_use = ap_get_force_use;
    dap->policy.set_can_mute_enforced_audible = ap_set_can_mute_enforced_audible;
    dap->policy.init_check = ap_init_check;
    dap->policy.get_output = ap_get_output;
    dap->policy.start_output = ap_start_output;
    dap->policy.stop_output = ap_stop_output;
    dap->policy.release_output = ap_release_output;
    dap->policy.get_input = ap_get_input;
    dap->policy.start_input = ap_start_input;
    dap->policy.stop_input = ap_stop_input;
    dap->policy.release_input = ap_release_input;
    dap->policy.init_stream_volume = ap_init_stream_volume;
    dap->policy.set_stream_volume_index = ap_set_stream_volume_index;
    dap->policy.get_stream_volume_index = ap_get_stream_volume_index;
#ifndef ICS_AUDIO_BLOB
    dap->policy.set_stream_volume_index_for_device = ap_set_stream_volume_index_for_device;
    dap->policy.get_stream_volume_index_for_device = ap_get_stream_volume_index_for_device;
#endif
    dap->policy.get_strategy_for_stream = ap_get_strategy_for_stream;
    dap->policy.get_devices_for_stream = ap_get_devices_for_stream;
    dap->policy.get_output_for_effect = ap_get_output_for_effect;
    dap->policy.register_effect = ap_register_effect;
    dap->policy.unregister_effect = ap_unregister_effect;
    dap->policy.set_effect_enabled = ap_set_effect_enabled;
    dap->policy.is_stream_active = ap_is_stream_active;
#ifndef ICS_AUDIO_BLOB
     // No NULL check in AudioPolicyService.cpp
    dap->policy.is_stream_active_remotely = ap_is_stream_active_remotely;
    dap->policy.is_source_active = NULL;
#endif
    dap->policy.dump = ap_dump;

    dap->service = service;
    dap->aps_ops = aps_ops;

    *ap = &dap->policy;
    return 0;
}

static int destroy_wrapper_ap(const struct audio_policy_device *ap_dev,
                              struct audio_policy *ap)
{
    struct wrapper_ap_device *dev;
    struct wrapper_audio_policy *policy;

    dev = (struct wrapper_ap_device *)ap_dev;
    policy = (struct wrapper_audio_policy *)ap;

    dev->vendor_device->destroy_audio_policy(dev->vendor_device,
                                            policy->vendor_policy);
    free(policy->wrapped_aps);

    free(ap);
    return 0;
}

static int wrapper_ap_dev_close(hw_device_t* device)
{
    VENDOR(device)->common.close((hw_device_t*)&(VENDOR(device)));
    free(device);
    return 0;
}

static int wrapper_ap_dev_open(const hw_module_t* module, const char* name,
                               hw_device_t** device)
{
    int ret = 0;
    struct wrapper_ap_device *dev;

    ALOGW("Wrapping vendor audio policy");

    *device = NULL;

    if (strcmp(name, AUDIO_POLICY_INTERFACE) != 0)
        return -EINVAL;

    dev = (struct wrapper_ap_device*) calloc(1, sizeof(struct wrapper_ap_device));
    if (!dev)
        return -ENOMEM;

    /* TODO: Move vendor- prefix into function */
    if (load_vendor_module("vendor-audio_policy", (const hw_module_t **) &gVendorModule)) {
        ALOGE("Failed to load vendor module");
        free(dev);
        return -EINVAL;
    }

    ALOGW("Wrapping vendor audio policy");

    ret = gVendorModule->common.methods->open((const hw_module_t*)gVendorModule,
                                             name, (hw_device_t**)&(dev->vendor_device));
    if(ret) {
        ALOGE("vendor audio_policy open fail");
        free(dev);
        return ret;
    }

    ALOGW("Wrapping vendor audio policy");

    dev->device.common.tag = HARDWARE_DEVICE_TAG;
    dev->device.common.version = 0;
    dev->device.common.module = (hw_module_t *)module;
    dev->device.common.close = wrapper_ap_dev_close;
    dev->device.create_audio_policy = create_wrapper_ap;
    dev->device.destroy_audio_policy = destroy_wrapper_ap;

    ALOGW("Wrapping vendor audio policy");

    *device = &dev->device.common;

    ALOGW("Wrapping vendor audio policy");

    return 0;
}

static struct hw_module_methods_t wrapper_ap_module_methods = {
    .open = wrapper_ap_dev_open,
};

struct wrapper_ap_module HAL_MODULE_INFO_SYM = {
    .module = {
        .common = {
            .tag            = HARDWARE_MODULE_TAG,
            .version_major  = 1,
            .version_minor  = 0,
            .id             = AUDIO_POLICY_HARDWARE_MODULE_ID,
            .name           = "Wrapper audio policy HAL",
            .author         = "The Android Open Source Project",
            .methods        = &wrapper_ap_module_methods,
            .dso            = NULL,
            .reserved       = {0},
        },
    },
};
