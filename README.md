
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


