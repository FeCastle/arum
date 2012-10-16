#! /bin/sh

## file: load_hwCtr.sh
## Description: Run this script to load the hardware counter module and
##              create the device file
## Note: This script uses insmod to load the module from the current
##       directory.
## Usage: ./load_hwCtr [parameters]

module="HwCtr"
device="Arum"
mode="664"
group="users"

## load the module with any module parameters that have been provided
## If not able to load, then exit
/sbin/insmod ./$module.ko $* || exit 1

## retrieve major number
major=$(awk "\$2==\"$module\" {print \$1}" /proc/devices)

## Do some clean up of device files, just incase
rm -f /dev/${device}0

## make device file
mknod /dev/${device}0 c $major 0

## create a link
ln -sf ${device}0 /dev/${device}

chgrp $group /dev/${device}0
chmod $mode  /dev/${device}0

