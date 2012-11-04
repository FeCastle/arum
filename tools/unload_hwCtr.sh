#! /bin/sh

## file: unload_hwCtr.sh
## Description: Run this script to unload the hardware counter module and
##              remove the device file
## Note: This script uses rmmod to unload the module; assumes device file
##       is in /dev
## Usage: ./unload_hwCtr.sh

module="HwCtr"
device="Arum"

## unload the module
## If not able to unload, then exit
/sbin/rmmod ./$module $* || exit 1

## Remove device file
rm -f /dev/${device} /dev/${device}0

