#!/bin/sh

rm -rf /configs/stream_hack

if [ -w /configs/wyze_hack.sh.orig ]; then
        mv /configs/wyze_hack.sh.orig /configs/wyze_hack.sh
fi

system_read_only=$(grep -w /system /proc/mounts | grep -w -c ro)

if [ -w /system/init/app_init.sh.orig ] && [ $system_read_only == 0 ] ; then
        mv /system/init/app_init.sh.orig /system/init/app_init.sh
fi
