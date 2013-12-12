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


#if WRAPPED_AUDIO_HAL_VERSION < ANDROID_VERSION(4, 2)
#define CONVERT_AUDIO_DEVICES_T
#endif

#if WRAPPED_AUDIO_HAL_VERSION == ANDROID_VERSION(4, 0)
#include "include/4.0/hardware/audio.h"
#elseif WRAPPED_AUDIO_HAL_VERSION == ANDROID_VERSION(4, 1)
#include "include/4.1/hardware/audio.h"
#elseif WRAPPED_AUDIO_HAL_VERSION == ANDROID_VERSION(4, 2)
#include "include/4.2/hardware/audio.h"
#elseif WRAPPED_AUDIO_HAL_VERSION == ANDROID_VERSION(4, 3)
#include "include/4.3/hardware/audio.h"
#endif
