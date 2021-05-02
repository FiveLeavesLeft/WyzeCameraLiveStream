
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
