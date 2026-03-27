#!/bin/bash
export LD_LIBRARY_PATH=$PWD/lib/
export KTD_ROOT=/opt/usr/ap_adsfi
$PWD/avos_filespace_manager > $PWD/log.txt &
