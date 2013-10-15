/*
 * Copyright (C) 2011 The Android Open Source Project
 * Copyright (c) 2012, Code Aurora Forum. All rights reserved.
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


#ifndef ICS_ANDROID_AUDIO_POLICY_INTERFACE_H
#define ICS_ANDROID_AUDIO_POLICY_INTERFACE_H

struct ics_audio_policy {
    /*
     * configuration functions
     */

    /* indicate a change in device connection status */
    int (*set_device_connection_state)(struct ics_audio_policy *pol,
                                       audio_devices_t device,
                                       audio_policy_dev_state_t state,
                                       const char *device_address);

    /* retrieve a device connection status */
    audio_policy_dev_state_t (*get_device_connection_state)(
                                            const struct ics_audio_policy *pol,
                                            audio_devices_t device,
                                            const char *device_address);

    /* indicate a change in phone state. Valid phones states are defined
     * by audio_mode_t */
    void (*set_phone_state)(struct ics_audio_policy *pol, audio_mode_t state);

    /* deprecated, never called (was "indicate a change in ringer mode") */
    void (*set_ringer_mode)(struct ics_audio_policy *pol, uint32_t mode,
                            uint32_t mask);

    /* force using a specific device category for the specified usage */
    void (*set_force_use)(struct ics_audio_policy *pol,
                          audio_policy_force_use_t usage,
                          audio_policy_forced_cfg_t config);

    /* retrieve current device category forced for a given usage */
    audio_policy_forced_cfg_t (*get_force_use)(const struct ics_audio_policy *pol,
                                               audio_policy_force_use_t usage);

    /* if can_mute is true, then audio streams that are marked ENFORCED_AUDIBLE
     * can still be muted. */
    void (*set_can_mute_enforced_audible)(struct ics_audio_policy *pol,
                                          bool can_mute);

    /* check proper initialization */
    int (*init_check)(const struct ics_audio_policy *pol);

    /*
     * Audio routing query functions
     */

    /* request an output appropriate for playback of the supplied stream type and
     * parameters */
    audio_io_handle_t (*get_output)(struct ics_audio_policy *pol,
                                    audio_stream_type_t stream,
                                    uint32_t samplingRate,
                                    audio_format_t format,
                                    audio_channel_mask_t channelMask,
                                    audio_output_flags_t flags);

    /* indicates to the audio policy manager that the output starts being used
     * by corresponding stream. */
    int (*start_output)(struct ics_audio_policy *pol,
                        audio_io_handle_t output,
                        audio_stream_type_t stream,
                        int session);

    /* indicates to the audio policy manager that the output stops being used
     * by corresponding stream. */
    int (*stop_output)(struct ics_audio_policy *pol,
                       audio_io_handle_t output,
                       audio_stream_type_t stream,
                       int session);

    /* releases the output. */
    void (*release_output)(struct ics_audio_policy *pol, audio_io_handle_t output);

    /* request an input appropriate for record from the supplied device with
     * supplied parameters. */
    audio_io_handle_t (*get_input)(struct ics_audio_policy *pol, audio_source_t inputSource,
                                   uint32_t samplingRate,
                                   audio_format_t format,
                                   audio_channel_mask_t channelMask,
                                   audio_in_acoustics_t acoustics);

    /* indicates to the audio policy manager that the input starts being used */
    int (*start_input)(struct ics_audio_policy *pol, audio_io_handle_t input);

    /* indicates to the audio policy manager that the input stops being used. */
    int (*stop_input)(struct ics_audio_policy *pol, audio_io_handle_t input);

    /* releases the input. */
    void (*release_input)(struct ics_audio_policy *pol, audio_io_handle_t input);

    /*
     * volume control functions
     */

    /* initialises stream volume conversion parameters by specifying volume
     * index range. The index range for each stream is defined by AudioService. */
    void (*init_stream_volume)(struct ics_audio_policy *pol,
                               audio_stream_type_t stream,
                               int index_min,
                               int index_max);

    /* sets the new stream volume at a level corresponding to the supplied
     * index. The index is within the range specified by init_stream_volume() */
    int (*set_stream_volume_index)(struct ics_audio_policy *pol,
                                   audio_stream_type_t stream,
                                   int index);

    /* retrieve current volume index for the specified stream */
    int (*get_stream_volume_index)(const struct ics_audio_policy *pol,
                                   audio_stream_type_t stream,
                                   int *index);

    /* return the strategy corresponding to a given stream type */
    uint32_t (*get_strategy_for_stream)(const struct ics_audio_policy *pol,
                                        audio_stream_type_t stream);

    /* return the enabled output devices for the given stream type */
    audio_devices_t (*get_devices_for_stream)(const struct ics_audio_policy *pol,
                                       audio_stream_type_t stream);

    /* Audio effect management */
    audio_io_handle_t (*get_output_for_effect)(struct ics_audio_policy *pol,
                                            const struct effect_descriptor_s *desc);

    int (*register_effect)(struct ics_audio_policy *pol,
                           const struct effect_descriptor_s *desc,
                           audio_io_handle_t output,
                           uint32_t strategy,
                           int session,
                           int id);

    int (*unregister_effect)(struct ics_audio_policy *pol, int id);

    int (*set_effect_enabled)(struct ics_audio_policy *pol, int id, bool enabled);

    bool (*is_stream_active)(const struct ics_audio_policy *pol,
            audio_stream_type_t stream,
            uint32_t in_past_ms);

    /* dump state */
    int (*dump)(const struct ics_audio_policy *pol, int fd);
};

struct ics_audio_policy_device {
    struct hw_device_t common;

    int (*create_audio_policy)(const struct ics_audio_policy_device *device,
                               struct audio_policy_service_ops *aps_ops,
                               void *service,
                               struct ics_audio_policy **ap);

    int (*destroy_audio_policy)(const struct ics_audio_policy_device *device,
                                struct ics_audio_policy *ap);
};

#endif  // ICS_ANDROID_ICS_AUDIO_POLICY_INTERFACE_H
