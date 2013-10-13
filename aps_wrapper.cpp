#define LOG_TAG "AudioPolicyServiceWrapper"

#include <cutils/log.h>

#include "aps_wrapper.h"
#include "common.h"

#define WAPS(s) ((struct wrapped_aps *) service)

#define WRAPPED_CALL(service, func, ...) ({\
    ALOGW("%s", __FUNCTION__); \
    struct wrapped_aps * __wrapped_aps = (struct wrapped_aps*) service; \
    return __wrapped_aps->aps_ops->func(__wrapped_aps->service, ##__VA_ARGS__); \
})


static audio_module_handle_t aps_load_hw_module(void *service,
                                             const char *name)
{
    WRAPPED_CALL(service, load_hw_module, name);
}

// deprecated: replaced by aps_open_output_on_module()
static audio_io_handle_t aps_open_output(void *service,
                                         audio_devices_t *pDevices,
                                         uint32_t *pSamplingRate,
                                         audio_format_t *pFormat,
                                         audio_channel_mask_t *pChannelMask,
                                         uint32_t *pLatencyMs,
                                         audio_output_flags_t flags)
{
    ALOGW("%s: 0x%x, %d, %d, %x, %d, %x", __FUNCTION__, *pDevices, *pSamplingRate, *pFormat, *pChannelMask, *pLatencyMs, flags);
    audio_devices_t devices = convert_ics_to_jb(*pDevices);
    
    /* The old NVIDIA policy manager doesn't set the PRIMARY flag for the 
     * first output it opens. */
    uint32_t newflags = flags | AUDIO_OUTPUT_FLAG_PRIMARY;
    flags = (audio_output_flags_t)newflags;    

    WRAPPED_CALL(service, open_output, &devices, pSamplingRate, pFormat,
                 pChannelMask, pLatencyMs, flags);
}

static audio_io_handle_t aps_open_output_on_module(void *service,
                                                   audio_module_handle_t module,
                                                   audio_devices_t *pDevices,
                                                   uint32_t *pSamplingRate,
                                                   audio_format_t *pFormat,
                                                   audio_channel_mask_t *pChannelMask,
                                                   uint32_t *pLatencyMs,
                                                   audio_output_flags_t flags)
{
    ALOGW("%s: 0x%x, %d, %d, %x, %d, %x", __FUNCTION__, *pDevices, *pSamplingRate, *pFormat, *pChannelMask, *pLatencyMs, flags);
    audio_devices_t devices = convert_ics_to_jb(*pDevices);
    WRAPPED_CALL(service, open_output_on_module, module, &devices, pSamplingRate,
                 pFormat, pChannelMask, pLatencyMs, flags);
}

static audio_io_handle_t aps_open_dup_output(void *service,
                                                 audio_io_handle_t output1,
                                                 audio_io_handle_t output2)
{
    WRAPPED_CALL(service, open_duplicate_output, output1, output2);
}

static int aps_close_output(void *service, audio_io_handle_t output)
{
    WRAPPED_CALL(service, close_output, output);
}

static int aps_suspend_output(void *service, audio_io_handle_t output)
{
    WRAPPED_CALL(service, suspend_output, output);
}

static int aps_restore_output(void *service, audio_io_handle_t output)
{
    WRAPPED_CALL(service, restore_output, output);
}

// deprecated: replaced by aps_open_input_on_module(), and acoustics parameter is ignored
static audio_io_handle_t aps_open_input(void *service,
                                        audio_devices_t *pDevices,
                                        uint32_t *pSamplingRate,
                                        audio_format_t *pFormat,
                                        audio_channel_mask_t *pChannelMask,
                                        audio_in_acoustics_t acoustics)
{
    ALOGW("%s: 0x%x, %d, %d, %x, %d", __FUNCTION__, *pDevices, *pSamplingRate, *pFormat, *pChannelMask, acoustics);
    audio_devices_t devices = convert_ics_to_jb(*pDevices);
    WRAPPED_CALL(service, open_input, &devices, pSamplingRate, pFormat,
                 pChannelMask, acoustics);
}

static audio_io_handle_t aps_open_input_on_module(void *service,
                                                  audio_module_handle_t module,
                                                  audio_devices_t *pDevices,
                                                  uint32_t *pSamplingRate,
                                                  audio_format_t *pFormat,
                                                  audio_channel_mask_t *pChannelMask)
{
    ALOGW("%s: 0x%x, %d, %d, %x", __FUNCTION__, *pDevices, *pSamplingRate, *pFormat, *pChannelMask);
    audio_devices_t devices = convert_ics_to_jb(*pDevices);
    WRAPPED_CALL(service, open_input_on_module, module, &devices, pSamplingRate, pFormat,
                 pChannelMask);
}

static int aps_close_input(void *service, audio_io_handle_t input)
{
    WRAPPED_CALL(service, close_input, input);
}

static int aps_set_stream_output(void *service, audio_stream_type_t stream,
                                     audio_io_handle_t output)
{
    WRAPPED_CALL(service, set_stream_output, stream, output);
}

static int aps_move_effects(void *service, int session,
                                audio_io_handle_t src_output,
                                audio_io_handle_t dst_output)
{
    WRAPPED_CALL(service, move_effects, session, src_output, dst_output);
}

static char * aps_get_parameters(void *service, audio_io_handle_t io_handle,
                                     const char *keys)
{
    WRAPPED_CALL(service, get_parameters, io_handle, keys);
}

static void aps_set_parameters(void *service, audio_io_handle_t io_handle,
                                   const char *kv_pairs, int delay_ms)
{
    struct wrapped_aps * waps = (struct wrapped_aps*) service;

    ALOGW("%s: kv_pairs: %s", __FUNCTION__, kv_pairs);
    char * fixed_kv_pairs = fixup_audio_parameters(kv_pairs, false);
    ALOGW("%s: fixed_kv_pairs: %s", __FUNCTION__, fixed_kv_pairs);
    waps->aps_ops->set_parameters(waps->service, io_handle, fixed_kv_pairs, delay_ms);
    ALOGW("%s: fixed_kv_pairs: %s", __FUNCTION__, fixed_kv_pairs);

    free(fixed_kv_pairs);
}

static int aps_set_stream_volume(void *service, audio_stream_type_t stream,
                                     float volume, audio_io_handle_t output,
                                     int delay_ms)
{
    ALOGW("%s: stream: %d, volume: %f, output: %d, delay_ms: %d",
          __FUNCTION__, stream, volume, output, delay_ms);
    WRAPPED_CALL(service, set_stream_volume, stream, volume, output, delay_ms);
}

static int aps_start_tone(void *service, audio_policy_tone_t tone,
                              audio_stream_type_t stream)
{
    WRAPPED_CALL(service, start_tone, tone, stream);
}

static int aps_stop_tone(void *service)
{
    WRAPPED_CALL(service, stop_tone);
}

static int aps_set_voice_volume(void *service, float volume, int delay_ms)
{
    ALOGW("%s: volume: %f, delay_ms: %d", __FUNCTION__, volume, delay_ms);
    WRAPPED_CALL(service, set_voice_volume, volume, delay_ms);
}


int wrap_aps(void *service, struct audio_policy_service_ops * aps_ops, struct wrapped_aps ** wrapped_service) {
    struct wrapped_aps * waps;


    waps = (struct wrapped_aps *) malloc(sizeof(*waps));
    if(!waps)
        return -ENOMEM;

    waps->service = service;
    waps->aps_ops = aps_ops;

    waps->wrapped_aps_ops.open_output = aps_open_output;
    waps->wrapped_aps_ops.open_duplicate_output = aps_open_dup_output;
    waps->wrapped_aps_ops.close_output  = aps_close_output;
    waps->wrapped_aps_ops.suspend_output = aps_suspend_output;
    waps->wrapped_aps_ops.restore_output = aps_restore_output;
    waps->wrapped_aps_ops.open_input  = aps_open_input;
    waps->wrapped_aps_ops.close_input  = aps_close_input;
    waps->wrapped_aps_ops.set_stream_volume = aps_set_stream_volume;
    waps->wrapped_aps_ops.set_stream_output = aps_set_stream_output;
    waps->wrapped_aps_ops.set_parameters = aps_set_parameters;
    waps->wrapped_aps_ops.get_parameters = aps_get_parameters;
    waps->wrapped_aps_ops.start_tone  = aps_start_tone;
    waps->wrapped_aps_ops.stop_tone  = aps_stop_tone;
    waps->wrapped_aps_ops.set_voice_volume = aps_set_voice_volume;
    waps->wrapped_aps_ops.move_effects  = aps_move_effects;
    waps->wrapped_aps_ops.load_hw_module = aps_load_hw_module;
    waps->wrapped_aps_ops.open_output_on_module = aps_open_output_on_module;
    waps->wrapped_aps_ops.open_input_on_module = aps_open_input_on_module;

   *wrapped_service = waps;

   return 0;
}
