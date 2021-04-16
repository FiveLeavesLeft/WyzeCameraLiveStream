#!/bin/sh -x

# These are useful for debug. You shouldn't have to change anything.

# Uncomment this if you want to browse the output of iCamera
# Useful for debugging.
#WYZE_APP_OUTPUT=/tmp/zz

# Uncomment this is you want to borwse the output of the stream hack
# Useful for debugging.
STREAM_HACK_OUTPUT=/tmp/stream_hack.log

# Uncomment this is if you want to set the port the hack listens on
#STREAM_HACK_PORT=12345

# Uncomment this is if you want change the limp
# STREAM_HACK_LIB='www.sonic.net/~crb/libimp.so'

# Uncomment this is if you want to get the dev version
# STREAM_HACK_VERSION='dev'

# Experimental: stream_hack over p2p
#STREAM_HACK_P2P='165.232.153.160:5005'

# DO NOT EDIT BELOW THIS LINE.

if [ -z "$WYZE_APP_OUTPUT" ]; then
        WYZE_APP_OUTPUT=/dev/null
fi
if [ -z "$STREAM_HACK_OUTPUT" ]; then
        STREAM_HACK_OUTPUT=/dev/null
fi
if [ -z "$STREAM_HACK_VERSION" ]; then
        STREAM_HACK_VERSION=current
fi
if [ -z "$STREAM_HACK_LIB" ]; then
        STREAM_HACK_LIB="crb.users.sonic.net/$STREAM_HACK_VERSION/libimp.so"
fi

dir=/configs/stream_hack
rm -rf $dir
mkdir $dir
wget -O $dir/libimp.so $STREAM_HACK_LIB

wrap() {
        if ! grep stream_hack $1 > /dev/null ; then
                mv $1 $1.orig
        fi
        echo "#!/bin/sh" > $1
        echo "export LD_LIBRARY_PATH=/configs/stream_hack:\$LD_LIBRARY_PATH" >> $1
        echo "export STREAM_HACK_PORT=$STREAM_HACK_PORT" >> $1
        echo "export STREAM_HACK_P2P=$STREAM_HACK_P2P" >> $1
        echo "export STREAM_HACK_OUTPUT=$STREAM_HACK_OUTPUT" >> $1
        echo "$1.orig >& $WYZE_APP_OUTPUT &" >> $1
        chmod +x $1 $1.orig
}

if [ -w /configs/wyze_hack.sh ]; then
        wrap /configs/wyze_hack.sh
fi

system_read_only=$(grep -w /system /proc/mounts | grep -w -c ro)

if [ -w /system/init/app_init.sh ] && [ $system_read_only == 0 ] ; then
        wrap /system/init/app_init.sh
fi
