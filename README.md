
# Wyze Cam Live Streaming 

This is a hack to allow live streaming from a wyze cam on your local network.

# Installation 

Installation currenty requires telnet on the camera. V3 cameras
require wyze_hacks installed. ( Write more about this. )

	telnet <your cam ip>
	
Once on your camera:

	cd /configs
	wget www.sonic.net/~crb/get
	chmod +x get
	./get

Then reboot or power cycle the camera and try:

	mpv http://<your cam ip>:12345
        #mov http://192.168.0.124:12345
        #mov http://192.168.0.121:12345

or

        vlc http://<your cam ip>:12345 --demux h264
        #vlc http://192.168.0.124:12345 --demux h264
        #vlc http://192.168.0.121:12345 --demux h264

	( on MacOs alias vlc='/Applications/VLC.app/Contents/MacOS/VLC' )


# How it works:

The hack modifies LD_LIBRARY_PATH and provides an alternative libimp.so.

On V2 cameras, 'get' wraps /system/init/app_init.sh 

On V3 cameras, 'get' wraps /configs/wyze_hack.sh

Both versions 'get' creates the directory /configs/stream_hack and puts the
alternative vesion of libimp.so there.

crb@sonic.net
Fri Apr  9 20:36:13 UTC 2021 

Tested on Wyze V2 with firmware 4.9.6.241 and VLC 3.0.10.
# repo
