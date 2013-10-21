# Set this to true if framework is build with ICS_AUDIO_BLOB defined. If false
# the audio_devices enum is converted between Android <= 4.1 and >= 4.2 API
# values.
FRAMEWORK_EXPECTS_ICS_AUDIO_BLOB := false

# In the case we are using the stock Android audio policy we have to know if we
# are using an HTC ICS audio primary blob to work around a problem when opening
# input streams.
HTC_ICS_AUDIO_BLOB := true

BUILD_AUDIO_POLICY_WRAPPER := false
BUILD_AUDIO_HW_WRAPPER := true
