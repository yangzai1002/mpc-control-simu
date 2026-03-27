#!/bin/bash

mkdir -p /opt/usr/ap_adsfi/bag/

source /opt/usr/ap_adsfi/script/env.sh
AP_BAG record \
/tpimage_list \ 
/tpcompressed_image_list  \
/tpwheelspeed   \
/tpsensorgps  \
/tpsensorimu   \
/tpimu   \
/tpperception_vp_result   \
/tplidarobjects   \
/tpterrainmap   \
/tpsemanticmap   
/tpperceptionobjects   \
/tptarget_obj   \
/tpododr   \
/tplocalization   \
/tpplanningstatus_follow  \ 
/tppathplan_follow   \
/tppathplan_auto  \ 
/tpplanningstatus_rc  \ 
/tpremote_path \
/tpbusiness_cmd   \
/tpvideo_profile   \
/tptask_manage_status  \
/tptask_cmd    \
/tptask_execute_status    \
/tpvideo_encoded_frame   \
/tpfront_left_lidar_packets   \
/tpfront_right_lidar_packets   \
/tpfront_middle_lidar_packets   \
/tpfront_middle_radar_packets   \
/tpfront_left_radar_packets   \
/tpfront_right_radar_packets   \
/tpimage_for_push    \
--split --size=100  -o adsfi_bag_  -p /opt/usr/ap_adsfi/bag/  > $PWD/log.txt  &

cd ../avos_filespace_manager/ && bash ./avos_filespace_manager.sh & 

