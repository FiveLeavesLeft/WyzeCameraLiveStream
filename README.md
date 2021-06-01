
# Wyze Cam Live Streaming 

This is a hack to allow live streaming from a wyze cam on your local
network.

# Installation - New!

The simplest fix for wyze events not being detected was to integrate
the stream hack into WyzeHacks so I forked WyzeHacks into 
https://github.com/FiveLeavesLeft/WyzeHacks and also included the
zip file here in this directory.

To install, download the zip file and:

	unzip wyze_hacks_0_5_06.zip
	cp config.inc.TEMPLATE config.inc
	./remote_install.sh

# Third-Party Integration/Configuration

## Homebridge/HOOBS

Install the [Homebridge Camera FFmpeg](https://github.com/Sunoo/homebridge-camera-ffmpeg) plugin.

Sample configuration:

```json
    {
        "name": "CAM-NAME",
        "manufacturer": "Wyze",
        "model": "Cam v2",
        "unbridge": true,
        "videoConfig": {
            "source": "-i http://CAM-IP:12345",
            "stillImageSource": "-i http://CAM-IP:12345 -vframes 1 -r 1",
            "vcodec": "copy",
            "maxStreams": 1,
            "audio": false,
            "packetSize": 188
        }
    }
```
[Source: @mrlt8](https://github.com/FiveLeavesLeft/WyzeCameraLiveStream/issues/5#issuecomment-831626312)

### Enable Motion in Homebridge
- Configure **HTTP Port** in **Camera FFMPEG**
- Set `"motion": true,` for your camera in **Camera FFMPEG**
- Configure the following script and copy it to your camera (e.g. /media/mmc/scripts/motion.sh):

```bash
#!/bin/sh
SLEEP=10
BRIDGE=homebridge-ip:port
CAMERA=camera-name-in-homebridge
while : ; do
    sleep $SLEEP                                       
    [[ ! -f /tmp/*.jpg ]] && continue              
     wget -q -O - "http://$BRIDGE/motion?$CAMERA"
done 
```
You can auto start the script by calling it from `export CUSTOM_SCRIPT=` in `/configs/wyze_hack.cfg`

[Source: @mrlt8](https://github.com/FiveLeavesLeft/WyzeCameraLiveStream/issues/5#issuecomment-850953208)

---

## Home Assistant

Sample Configuration:

```yaml
wyzefront:
    ffmpeg:
      # hwaccel_args:
      #   - -hwaccel
      #   - vaapi
      #   - -hwaccel_device
      #   - /dev/dri/renderD128
      #   - -hwaccel_output_format
      #   - yuv420p
      input_args:
        - -avoid_negative_ts
        - make_zero
      inputs:
        - path: http://x.x.x.x:12345
          roles:
            - detect
            - clips
            - rtmp
    width: 1920
    height: 1080
    fps: 5
    clips:
      # Required: enables clips for the camera (default: shown below)
      # This value can be set via MQTT and will be updated in startup based on retained value
      enabled: True
      # Optional: Number of seconds before the event to include in the clips (default: shown below)
      pre_capture: 5
      # Optional: Number of seconds after the event to include in the clips (default: shown below)
      post_capture: 5
      # Optional: Objects to save clips for. (default: all tracked objects)
      objects:
        - person
```
[Source: @romedtino](https://github.com/FiveLeavesLeft/WyzeCameraLiveStream/issues/5#issuecomment-832374049)


---

## Blue Iris 
Sample Connfig:

- Make: **Generic/ONVIF**
- Model: **HTTP Live Streaming (HLS, M3U8), MP2TS**
- Media/video/RTSP port: **12345**

![config](https://camo.githubusercontent.com/15fe27c4b53d111eab5bb93c6eebfa2e5170cddf8cb7375cac3d4beea0686641/68747470733a2f2f696d6775722e636f6d2f326677563732702e706e67)

[Source: @dudududodododedede](https://github.com/FiveLeavesLeft/WyzeCameraLiveStream/issues/9#issuecomment-835694551)

---

# Installation - ( Old, no longer supported )

Installation currenty requires telnet on the camera. The
WyzeHacks github repository will let you install telnet. For details
see Telnet.md in this directory.

Telnet to your camera, login in as root, the password is probably ismart12.

	telnet <your cam ip>
	
And once on your camera:

	cd /configs
	wget crb.users.sonic.net/current/install.sh
	chmod +x install.sh
	./install.sh

Then reboot or power cycle the camera and try one of these on your desktop:

	mpv http://<your cam ip>:12345
        eg. mpv http://192.168.0.124:12345

or

        vlc http://<your cam ip>:12345 --demux h264
        eg. vlc http://192.168.0.124:12345 --demux h264

( On MacOs alias vlc='/Applications/VLC.app/Contents/MacOS/VLC' )

# To uninstall

Telnet to your camera and:

	cd /configs
	wget crb.users.sonic.net/current/uninstall.sh
	chmod +x uninstall.sh
	./uninstall.sh

# Why is it installed from sonic.net and not github?

The version of wget on the camera does not support https.

# Known issues

There have been some reports of wyze events not being uploaded to the
cloud. This has been addressed but it depended on the firmware version;
you can check on your camera by waving your hand in front of the camera
and see if the event gets uploaded to the cloud.


# How it works:

The hack modifies LD_LIBRARY_PATH and provides an alternative libimp.so.

On V2 cameras, install.sh wraps /system/init/app_init.sh 

On V3 cameras, install.sh wraps /configs/wyze_hack.sh

install.sh creates the directory /configs/stream_hack and puts the
alternative vesion of libimp.so there.

# Toolchain

The toolchain I used is:

<https://sourcery.sw.siemens.com/GNUToolchain/release3217>

Documentation is here:

<https://sourcery.sw.siemens.com/GNUToolchain/doc23862/getting-started.pdf>
