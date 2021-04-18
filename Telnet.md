# Getting telnet on your camera

To install the stream hack on your camera you need telnet access.

You can get telnet access using the WyzeHacks project

	<https://github.com/HclX/WyzeHacks>

Download and installation information:

	<https://github.com/HclX/WyzeHacks#download-and-installation>

For V3 cameras, WyzeHacks currently requires firmware 4.36.0.228 or later, 
to be installed on your camera.

As of this writing, the current version of WyzeHacks is 0.5.05; these
commands worked for me:

	mkdir tmp
	cd tmp
	wget https://github.com/HclX/WyzeHacks/raw/master/release/wyze_hacks_0_5_05.zip
	unzip wyze_hacks_0_5_05.zip
	cp config.inc.TEMPLATE config.inc
	./remote_install.sh

You can read about this here:

	<https://github.com/HclX/WyzeHacks#download-and-installation>

