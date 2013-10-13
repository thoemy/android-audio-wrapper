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

#define LOG_TAG "AudioWrapper"
//#define LOG_NDEBUG 0

#include <errno.h>
#include <pthread.h>
#include <stdint.h>
#include <sys/time.h>

#include <cutils/log.h>

#include "common.h"
#include "ics_audio.h"

static struct audio_module *global_vendor_module = 0;

struct wrapper_audio_device {
    struct audio_hw_device device;
    struct ics_audio_hw_device *vendor_device;
};

struct wrapper_stream_out {
    struct audio_stream_out stream;
    struct ics_audio_stream_out *vendor_stream;
};

struct wrapper_stream_in {
    struct audio_stream_in stream;
    struct ics_audio_stream_in *vendor_stream;
};


#define DEVICE(d) (((struct wrapper_audio_device*) d)->device)
#define VENDOR(d) (((struct wrapper_audio_device*) d)->vendor_device)

#define VENDOR_STREAM_OUT(p) (((struct wrapper_stream_out*) p)->vendor_stream)
#define VENDOR_STREAM_IN(p) (((struct wrapper_stream_in*) p)->vendor_stream)

static uint32_t out_get_sample_rate(const struct audio_stream *stream)
{
    ALOGW("%s", __FUNCTION__);
    return VENDOR_STREAM_OUT(stream)->common.get_sample_rate(&(VENDOR_STREAM_OUT(stream)->common));
}

static int out_set_sample_rate(struct audio_stream *stream, uint32_t rate)
{
    ALOGW("%s", __FUNCTION__);
    return VENDOR_STREAM_OUT(stream)->common.set_sample_rate(&(VENDOR_STREAM_OUT(stream)->common), rate);
}

static size_t out_get_buffer_size(const struct audio_stream *stream)
{
    ALOGW("%s", __FUNCTION__);
    return VENDOR_STREAM_OUT(stream)->common.get_buffer_size(&(VENDOR_STREAM_OUT(stream)->common));
}

static audio_channel_mask_t out_get_channels(const struct audio_stream *stream)
{
    ALOGW("%s", __FUNCTION__);
    return VENDOR_STREAM_OUT(stream)->common.get_channels(&(VENDOR_STREAM_OUT(stream)->common));
}

static audio_format_t out_get_format(const struct audio_stream *stream)
{
    ALOGW("%s", __FUNCTION__);
    return VENDOR_STREAM_OUT(stream)->common.get_format(&(VENDOR_STREAM_OUT(stream)->common));
}

static int out_set_format(struct audio_stream *stream, audio_format_t format)
{
    ALOGW("%s", __FUNCTION__);
    return VENDOR_STREAM_OUT(stream)->common.set_format(&(VENDOR_STREAM_OUT(stream)->common), format);
}

static int out_standby(struct audio_stream *stream)
{
    ALOGW("%s", __FUNCTION__);
    return VENDOR_STREAM_OUT(stream)->common.standby(&(VENDOR_STREAM_OUT(stream)->common));
}

static int out_dump(const struct audio_stream *stream, int fd)
{
    ALOGW("%s", __FUNCTION__);
    return VENDOR_STREAM_OUT(stream)->common.dump(&(VENDOR_STREAM_OUT(stream)->common), fd);
}

static int out_set_parameters(struct audio_stream *stream, const char *kvpairs)
{
    int ret;

    ALOGW("%s: kvpairs: %s", __FUNCTION__, kvpairs);
    char * fixed_kvpairs = fixup_audio_parameters(kvpairs, true);
    ALOGW("%s: fixed_kvpairs: %s", __FUNCTION__, fixed_kvpairs);
    ret = VENDOR_STREAM_OUT(stream)->common.set_parameters(&(VENDOR_STREAM_OUT(stream)->common), fixed_kvpairs);

    free(fixed_kvpairs);
    return ret;
}

static char * out_get_parameters(const struct audio_stream *stream, const char *keys)
{
    char * ret;
    ALOGW("%s: keys: %s", __FUNCTION__, keys);
    ret = VENDOR_STREAM_OUT(stream)->common.get_parameters(&(VENDOR_STREAM_OUT(stream)->common), keys);
    ALOGW("%s: values: %s", __FUNCTION__, ret);
    return ret;
}

static uint32_t out_get_latency(const struct audio_stream_out *stream)
{
    ALOGW("%s", __FUNCTION__);
    return VENDOR_STREAM_OUT(stream)->get_latency(VENDOR_STREAM_OUT(stream));
}

static int out_set_volume(struct audio_stream_out *stream, float left,
                          float right)
{
    ALOGW("%s", __FUNCTION__);
    return VENDOR_STREAM_OUT(stream)->set_volume(VENDOR_STREAM_OUT(stream), left, right);
}

static ssize_t out_write(struct audio_stream_out *stream, const void* buffer,
                         size_t bytes)
{
    ALOGD("%s", __FUNCTION__);
    return VENDOR_STREAM_OUT(stream)->write(VENDOR_STREAM_OUT(stream), buffer, bytes);
}

static int out_get_render_position(const struct audio_stream_out *stream,
                                   uint32_t *dsp_frames)
{
    ALOGW("%s", __FUNCTION__);
    return VENDOR_STREAM_OUT(stream)->get_render_position(VENDOR_STREAM_OUT(stream), dsp_frames);
}

static int out_add_audio_effect(const struct audio_stream *stream, effect_handle_t effect)
{
    ALOGW("%s", __FUNCTION__);
    return VENDOR_STREAM_OUT(stream)->common.add_audio_effect(&(VENDOR_STREAM_OUT(stream)->common), effect);
}

static int out_remove_audio_effect(const struct audio_stream *stream, effect_handle_t effect)
{
    ALOGW("%s", __FUNCTION__);
    return VENDOR_STREAM_OUT(stream)->common.remove_audio_effect(&(VENDOR_STREAM_OUT(stream)->common), effect);
}

#ifndef ICS_AUDIO_BLOB
static int out_get_next_write_timestamp(const struct audio_stream_out *stream,
                                        int64_t *timestamp)
{
    ALOGW("%s", __FUNCTION__);
    //return VENDOR_STREAM_OUT(stream)->get_next_write_timestamp(VENDOR_STREAM_OUT(stream), timestamp);
    return android::INVALID_OPERATION;
}
#endif

/** audio_stream_in implementation **/
static uint32_t in_get_sample_rate(const struct audio_stream *stream)
{
    ALOGW("%s", __FUNCTION__);
    return VENDOR_STREAM_IN(stream)->common.get_sample_rate(&(VENDOR_STREAM_IN(stream)->common));
}

static int in_set_sample_rate(struct audio_stream *stream, uint32_t rate)
{
    ALOGW("%s", __FUNCTION__);
    return VENDOR_STREAM_IN(stream)->common.set_sample_rate(&(VENDOR_STREAM_IN(stream)->common), rate);
}

static size_t in_get_buffer_size(const struct audio_stream *stream)
{
    ALOGW("%s", __FUNCTION__);
    return VENDOR_STREAM_IN(stream)->common.get_buffer_size(&(VENDOR_STREAM_IN(stream)->common));
}

static audio_channel_mask_t in_get_channels(const struct audio_stream *stream)
{
    ALOGW("%s", __FUNCTION__);
    return VENDOR_STREAM_IN(stream)->common.get_channels(&(VENDOR_STREAM_IN(stream)->common));
}

static audio_format_t in_get_format(const struct audio_stream *stream)
{
    ALOGW("%s", __FUNCTION__);
    return VENDOR_STREAM_IN(stream)->common.get_format(&(VENDOR_STREAM_IN(stream)->common));
}

static int in_set_format(struct audio_stream *stream, audio_format_t format)
{
    ALOGW("%s", __FUNCTION__);
    return VENDOR_STREAM_IN(stream)->common.set_format(&(VENDOR_STREAM_IN(stream)->common), format);
}

static int in_standby(struct audio_stream *stream)
{
    ALOGW("%s", __FUNCTION__);
    return VENDOR_STREAM_IN(stream)->common.standby(&(VENDOR_STREAM_IN(stream)->common));
}

static int in_dump(const struct audio_stream *stream, int fd)
{
    ALOGW("%s", __FUNCTION__);
    return VENDOR_STREAM_IN(stream)->common.dump(&(VENDOR_STREAM_IN(stream)->common), fd);
}

static int in_set_parameters(struct audio_stream *stream, const char *kvpairs)
{
    int ret;

    ALOGW("%s: kvpairs: %s", __FUNCTION__, kvpairs);
    char * fixed_kvpairs = fixup_audio_parameters(kvpairs, true);
    ALOGW("%s: fixed_kvpairs: %s", __FUNCTION__, fixed_kvpairs);
    ret = VENDOR_STREAM_IN(stream)->common.set_parameters(&(VENDOR_STREAM_IN(stream)->common), kvpairs);

    free(fixed_kvpairs);
    return ret;
}

static char * in_get_parameters(const struct audio_stream *stream,
                                const char *keys)
{
    char * ret;
    ALOGW("%s: keys: %s", __FUNCTION__, keys);
    ret = VENDOR_STREAM_IN(stream)->common.get_parameters(&(VENDOR_STREAM_IN(stream)->common), keys);
    ALOGW("%s: values: %s", __FUNCTION__, ret);
    return ret;
}

static int in_set_gain(struct audio_stream_in *stream, float gain)
{
    ALOGW("%s", __FUNCTION__);
    return VENDOR_STREAM_IN(stream)->set_gain(VENDOR_STREAM_IN(stream), gain);
}

static ssize_t in_read(struct audio_stream_in *stream, void* buffer,
                       size_t bytes)
{
    ssize_t ret;
    ALOGD("+++ %s", __FUNCTION__);
    ret = VENDOR_STREAM_IN(stream)->read(VENDOR_STREAM_IN(stream), buffer, bytes);
    ALOGD("--- %s, %d", __FUNCTION__, ret);
    return ret;
}

static uint32_t in_get_input_frames_lost(struct audio_stream_in *stream)
{
    ALOGW("%s", __FUNCTION__);
    return VENDOR_STREAM_IN(stream)->get_input_frames_lost(VENDOR_STREAM_IN(stream));
}

static int in_add_audio_effect(const struct audio_stream *stream, effect_handle_t effect)
{
    ALOGW("%s", __FUNCTION__);
    return VENDOR_STREAM_IN(stream)->common.add_audio_effect(&(VENDOR_STREAM_IN(stream)->common), effect);
}

static int in_remove_audio_effect(const struct audio_stream *stream, effect_handle_t effect)
{
    ALOGW("%s", __FUNCTION__);
    return VENDOR_STREAM_IN(stream)->common.remove_audio_effect(&(VENDOR_STREAM_IN(stream)->common), effect);
}

static int adev_open_output_stream(struct audio_hw_device *dev,
#ifndef ICS_AUDIO_BLOB
                              audio_io_handle_t handle,
                              audio_devices_t devices,
                              audio_output_flags_t flags,
                              struct audio_config *config,
                              struct audio_stream_out **stream_out)
#else
                              uint32_t devices,
                              int *format, uint32_t *channels,
                              uint32_t *sample_rate,
                              struct audio_stream_out **stream_out)
#endif
{
    struct wrapper_stream_out *out;
    int ret;
    ALOGW("%s", __FUNCTION__);

    out = (struct wrapper_stream_out *)calloc(1, sizeof(struct wrapper_stream_out));
    if (!out)
        return -ENOMEM;

    ALOGW("%s: devices 0x%x", __FUNCTION__, devices);
    devices = convert_jb_to_ics(devices);

#ifdef ICS_AUDIO_BLOB
    ret = VENDOR(dev)->open_output_stream(VENDOR(dev), devices, format, channels, sample_rate,
                                         &VENDOR_STREAM_OUT(out));
#else
    ret = VENDOR(dev)->open_output_stream(VENDOR(dev), devices, (int *)&config->format,
                                          &config->channel_mask, &config->sample_rate,
                                          &VENDOR_STREAM_OUT(out));
#endif

    ALOGW("%s: vendor_stream: %p", __FUNCTION__, VENDOR_STREAM_OUT(out));
    if(ret < 0)
        goto err_open;

    out->stream.common.get_sample_rate = out_get_sample_rate;
    out->stream.common.set_sample_rate = out_set_sample_rate;
    out->stream.common.get_buffer_size = out_get_buffer_size;
    out->stream.common.get_channels = out_get_channels;
    out->stream.common.get_format = out_get_format;
    out->stream.common.set_format = out_set_format;
    out->stream.common.standby = out_standby;
    out->stream.common.dump = out_dump;
    out->stream.common.set_parameters = out_set_parameters;
    out->stream.common.get_parameters = out_get_parameters;
    out->stream.common.add_audio_effect = out_add_audio_effect;
    out->stream.common.remove_audio_effect = out_remove_audio_effect;
    out->stream.get_latency = out_get_latency;
    out->stream.set_volume = out_set_volume;
    out->stream.write = out_write;
    out->stream.get_render_position = out_get_render_position;
#ifndef ICS_AUDIO_BLOB
    out->stream.get_next_write_timestamp = NULL; //out_get_next_write_timestamp;
#endif

    *stream_out = &out->stream;
    return 0;

err_open:
    free(out);
    *stream_out = NULL;
    return ret;
}

static void adev_close_output_stream(struct audio_hw_device *dev,
                                     struct audio_stream_out *stream)
{
    ALOGW("%s", __FUNCTION__);
    VENDOR(dev)->close_output_stream(VENDOR(dev), VENDOR_STREAM_OUT(stream));
    free(stream);
}

static int adev_set_parameters(struct audio_hw_device *dev, const char *kvpairs)
{
    ALOGW("%s: kvpairs: %s", __FUNCTION__, kvpairs);
    return VENDOR(dev)->set_parameters(VENDOR(dev), kvpairs);
}

static char * adev_get_parameters(const struct audio_hw_device *dev,
                                  const char *keys)
{
    char * ret;
    ALOGW("%s: keys: %s", __FUNCTION__, keys);
    ret = VENDOR(dev)->get_parameters(VENDOR(dev), keys);
    ALOGW("%s: values: %s", __FUNCTION__, ret);
    return ret;
}

static uint32_t adev_get_supported_devices(const struct audio_hw_device *dev)
{
    ALOGW("%s", __FUNCTION__);
    return convert_ics_to_jb(VENDOR(dev)->get_supported_devices(VENDOR(dev)));
}

static int adev_init_check(const struct audio_hw_device *dev)
{
    ALOGW("%s", __FUNCTION__);
    return VENDOR(dev)->init_check(VENDOR(dev));
}

static int adev_set_voice_volume(struct audio_hw_device *dev, float volume)
{
    ALOGW("%s", __FUNCTION__);
    return VENDOR(dev)->set_voice_volume(VENDOR(dev), volume);
}

static int adev_set_master_volume(struct audio_hw_device *dev, float volume)
{
    ALOGW("%s", __FUNCTION__);
    return VENDOR(dev)->set_master_volume(VENDOR(dev), volume);
}

#ifndef ICS_AUDIO_BLOB
static int adev_get_master_volume(struct audio_hw_device *dev, float *volume)
{
    ALOGW("%s", __FUNCTION__);
    //return VENDOR(dev)->get_master_volume(VENDOR(dev), volume);
    return 0;
}

static int adev_set_master_mute(struct audio_hw_device *dev, bool muted)
{
    ALOGW("%s", __FUNCTION__);
    //return VENDOR(dev)->set_master_mute(VENDOR(dev), muted);
    return 0;
}

static int adev_get_master_mute(struct audio_hw_device *dev, bool *muted)
{
    ALOGW("%s", __FUNCTION__);
    //return VENDOR(dev)->get_master_mute(VENDOR(dev), muted);
    return 0;
}
#endif

static int adev_set_mode(struct audio_hw_device *dev, audio_mode_t mode)
{
    ALOGW("%s", __FUNCTION__);
    return VENDOR(dev)->set_mode(VENDOR(dev), mode);
}

static int adev_set_mic_mute(struct audio_hw_device *dev, bool state)
{
    ALOGW("%s", __FUNCTION__);
    return VENDOR(dev)->set_mic_mute(VENDOR(dev), state);
}

static int adev_get_mic_mute(const struct audio_hw_device *dev, bool *state)
{
    ALOGW("%s", __FUNCTION__);
    return VENDOR(dev)->get_mic_mute(VENDOR(dev), state);
}


#ifndef ICS_AUDIO_BLOB
static size_t adev_get_input_buffer_size(const struct audio_hw_device *dev,
                                    const struct audio_config *config)
{
    ALOGW("%s", __FUNCTION__);
   return VENDOR(dev)->get_input_buffer_size(VENDOR(dev), config->sample_rate,
                                             config->format, popcount(config->channel_mask));
}
#else
static size_t adev_get_input_buffer_size(const struct audio_hw_device *dev,
                                    uint32_t sample_rate, int format,
                                    int channel_count)
{
    ALOGW("%s", __FUNCTION__);
    return VENDOR(dev)->get_input_buffer_size(VENDOR(dev), sample_rate, format, channel_count);
}
#endif

#ifndef ICS_AUDIO_BLOB
static int adev_open_input_stream(struct audio_hw_device *dev,
                             audio_io_handle_t handle,
                             audio_devices_t devices,
                             struct audio_config *config,
                             struct audio_stream_in **stream_in)
#else
static int adev_open_input_stream(struct audio_hw_device *dev, uint32_t devices,
                             int *format, uint32_t *channels,
                             uint32_t *sample_rate,
                             audio_in_acoustics_t acoustics,
                             struct audio_stream_in **stream_in)
#endif
{
    struct wrapper_stream_in *in;
    int ret;

    ALOGW("%s", __FUNCTION__);

    in = (struct wrapper_stream_in *)calloc(1, sizeof(struct wrapper_stream_in));
    if (!in)
        return -ENOMEM;
    
    ALOGW("%s: devices 0x%x", __FUNCTION__, devices);
    devices = convert_jb_to_ics(devices);

#ifndef ICS_AUDIO_BLOB
    ret = VENDOR(dev)->open_input_stream(VENDOR(dev), devices, (int *)&config->format, 
                                         &config->channel_mask, &config->sample_rate,
                                         (audio_in_acoustics_t)0, &VENDOR_STREAM_IN(in));
#else
    ret = VENDOR(dev)->open_input_stream(VENDOR(dev), devices, format, channels, sample_rate, acoustics,
                                         &VENDOR_STREAM_IN(in));
#endif
    if(ret < 0)
        goto err_open;

    in->stream.common.get_sample_rate = in_get_sample_rate;
    in->stream.common.set_sample_rate = in_set_sample_rate;
    in->stream.common.get_buffer_size = in_get_buffer_size;
    in->stream.common.get_channels = in_get_channels;
    in->stream.common.get_format = in_get_format;
    in->stream.common.set_format = in_set_format;
    in->stream.common.standby = in_standby;
    in->stream.common.dump = in_dump;
    in->stream.common.set_parameters = in_set_parameters;
    in->stream.common.get_parameters = in_get_parameters;
    in->stream.common.add_audio_effect = in_add_audio_effect;
    in->stream.common.remove_audio_effect = in_remove_audio_effect;
    in->stream.set_gain = in_set_gain;
    in->stream.read = in_read;
    in->stream.get_input_frames_lost = in_get_input_frames_lost;

    ALOGW("%s: get_sample_rate: %d", __FUNCTION__, in->stream.common.get_sample_rate(&in->stream.common));

    *stream_in = &in->stream;
    return 0;

err_open:
    free(in);
    *stream_in = NULL;
    return ret;
}

static void adev_close_input_stream(struct audio_hw_device *dev,
                                   struct audio_stream_in *in)
{
    ALOGW("%s", __FUNCTION__);
    VENDOR(dev)->close_input_stream(VENDOR(dev), VENDOR_STREAM_IN(in));
    ALOGW("%s: 0x%p", __FUNCTION__, VENDOR_STREAM_IN(in));
        
    free(in);
}

static int adev_dump(const audio_hw_device_t *device, int fd)
{
    ALOGW("%s", __FUNCTION__);
    return VENDOR(device)->dump(VENDOR(device), fd);
    
}

static int adev_close(hw_device_t *device)
{
    ALOGW("%s", __FUNCTION__);
    VENDOR(device)->common.close((hw_device_t*)&(VENDOR(device)));
    free(device);
    return 0;
}

static int adev_open(const hw_module_t* module, const char* name,
                     hw_device_t** device)
{
    struct wrapper_audio_device *adev;
    int ret;

    ALOGW("Wrapping vendor audio primary");

    if (strcmp(name, AUDIO_HARDWARE_INTERFACE) != 0)
        return -EINVAL;

    adev = (struct wrapper_audio_device *) calloc(1, sizeof(struct wrapper_audio_device));
    if (!adev)
        return -ENOMEM;

    /* TODO: Move vendor- prefix into function */
    if (load_vendor_module("vendor-audio.primary", (const hw_module_t **) &global_vendor_module)) {
        ALOGE("Failed to load vendor audio.primary module");
        free(adev);
        return -EINVAL;
    }

    ret = global_vendor_module->common.methods->open((const hw_module_t*)global_vendor_module,
                                             name, (hw_device_t**)&(adev->vendor_device));
    if(ret) {
        ALOGE("vendor audio.primary open fail");
        free(adev);
        return ret;
    }

    adev->device.common.tag = HARDWARE_DEVICE_TAG;
    adev->device.common.version = AUDIO_DEVICE_API_VERSION_2_0;
    adev->device.common.module = (struct hw_module_t *) module;
    adev->device.common.close = adev_close;

    adev->device.get_supported_devices = adev_get_supported_devices;
    adev->device.init_check = adev_init_check;
    adev->device.set_voice_volume = adev_set_voice_volume;
    adev->device.set_master_volume = adev_set_master_volume;
#ifndef ICS_AUDIO_BLOB
    adev->device.get_master_volume = NULL; //adev_get_master_volume;
    adev->device.set_master_mute = NULL; //adev_set_master_mute;
    adev->device.get_master_mute = NULL; //adev_get_master_mute;
#endif
    adev->device.set_mode = adev_set_mode;
    adev->device.set_mic_mute = adev_set_mic_mute;
    adev->device.get_mic_mute = adev_get_mic_mute;
    adev->device.set_parameters = adev_set_parameters;
    adev->device.get_parameters = adev_get_parameters;
    adev->device.get_input_buffer_size = adev_get_input_buffer_size;
    adev->device.open_output_stream = adev_open_output_stream;
    adev->device.open_input_stream = adev_open_input_stream;
    adev->device.close_output_stream = adev_close_output_stream;
    adev->device.close_input_stream = adev_close_input_stream;
    adev->device.dump = adev_dump;

    *device = &adev->device.common;

    return 0;
}

static struct hw_module_methods_t hal_module_methods = {
    .open = adev_open,
};

struct audio_module HAL_MODULE_INFO_SYM = {
    .common = {
        .tag = HARDWARE_MODULE_TAG,
        .module_api_version = AUDIO_MODULE_API_VERSION_0_1,
        .hal_api_version = HARDWARE_HAL_API_VERSION,
        .id = AUDIO_HARDWARE_MODULE_ID,
        .name = "LEGACY Audio HW HAL",
        .author = "The Android Open Source Project",
        .methods = &hal_module_methods,
        .dso = NULL,
        .reserved = {0},
    },
};
