#define LOG_TAG "AudioTest"

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <cutils/log.h>
#include <hardware/audio.h>
#include <sys/cdefs.h>

#include "include/4.2/hardware/audio.h"


#define MODULE_ID "vendor-" AUDIO_HARDWARE_MODULE_ID

int main(int argc, char** args)
{
    const hw_module_t *mod;
    audio_hw_device_t *dev;
    wrapper::audio_hw_device_t *legacy_dev;
    const char *if_name = "primary";
    char *module_id;
    int rc = 0;
    int fd;
    bool state;
    char *values;
    char *keys;
    

    if(argc <= 1 || argc > 2) {
        printf("Exactly one input argument expected!\n");
        rc = -1;
        goto out;
    }

    module_id = args[1];
    printf("Module id: %s", module_id);

    rc = hw_get_module_by_class(module_id, if_name, &mod);
    if (rc) {
        printf("Couldn't load audio hw module %s.%s (%s)\n",
                module_id, if_name, strerror(-rc));
        goto out;
    }

    ALOGW("Module loaded");

    rc = audio_hw_device_open(mod, &dev);
    if (rc || dev == NULL) {
        printf("Couldn't open audio hw device in %s.%s (%s)\n",
                module_id, if_name, strerror(-rc));

        goto out;
    }

    ALOGW("Device opened");
    
    legacy_dev = (wrapper::audio_hw_device_t *) dev;
    
    printf("Audio hw device version %04x\n", legacy_dev->common.version);

    printf("get_supported_devices : %p\n", legacy_dev->get_supported_devices);
    printf("init_check            : %p\n", legacy_dev->init_check);
    printf("set_voice_volume      : %p\n", legacy_dev->set_voice_volume);
    printf("set_master_volume     : %p\n", legacy_dev->set_master_volume);
    printf("get_master_volume     : %p\n", legacy_dev->get_master_volume);
    printf("unknown_func1         : %p\n", legacy_dev->unknown_func1);
    printf("set_mode              : %p\n", legacy_dev->set_mode);
    printf("set_mic_mute          : %p\n", legacy_dev->set_mic_mute);
    printf("get_mic_mute          : %p\n", legacy_dev->get_mic_mute);
    printf("set_parameters        : %p\n", legacy_dev->set_parameters);
    printf("get_parameters        : %p\n", legacy_dev->get_parameters);
    printf("get_input_buffer_size : %p\n", legacy_dev->get_input_buffer_size);
    printf("open_output_stream    : %p\n", legacy_dev->open_output_stream);
    printf("close_output_stream   : %p\n", legacy_dev->close_output_stream);
    printf("open_input_stream     : %p\n", legacy_dev->open_input_stream);
    printf("close_input_stream    : %p\n", legacy_dev->close_input_stream);
    printf("dump                  : %p\n", legacy_dev->dump);
    printf("set_master_mute       : %p\n", legacy_dev->set_master_mute);
    printf("get_master_mute       : %p\n", legacy_dev->get_master_mute);
    
    rc = legacy_dev->init_check(legacy_dev);
    if(rc) {
        printf("Error: init_check: %s (%d)\n", strerror(-rc), rc);
        goto out;
    }

    rc = legacy_dev->get_mic_mute(legacy_dev, &state);
    if(rc) {
        printf("Error: get_mic_mute: %s (%d)\n", strerror(-rc), rc);
        goto out;
    }
    printf("get_mic_mute: %d\n", state);

    keys = "nv_param_ulp_supported";
    //keys[1] = "nv_param_avp_decode_position";
    values = legacy_dev->get_parameters(legacy_dev, keys);
    printf("get_parameters: %s\n", values);
    free(values);    
    

    fd = open("/data/tmp/out.txt", O_WRONLY|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR);
    if(fd) {
        printf("File created!\n");
        ALOGW("dumping");
        //fprintf(fd, "Dump\n\n");
        legacy_dev->dump(legacy_dev, fd);
        ALOGW("dumped");
        close(fd);
    }

                                                         
 out:
    return rc;
    
}
