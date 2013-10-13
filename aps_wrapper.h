#include <hardware/hardware.h>

#include <system/audio.h>
#include <system/audio_policy.h>
#include <hardware/audio_policy.h>

struct wrapped_aps {
    void * service;
    struct audio_policy_service_ops * aps_ops;
    struct audio_policy_service_ops wrapped_aps_ops;
};

int wrap_aps(void *service, struct audio_policy_service_ops * aps_ops, struct wrapped_aps ** wrapped_service);
