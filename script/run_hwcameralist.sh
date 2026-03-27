export KTD_ROOT=/opt/usr/wx/newap2/
export PROJ_CFG_PATH=/opt/usr/wx/newap2/config
export ICAOS_NODE_MY_ROOT=/opt/usr/wx/newap2/config/hardware_abstraction/hw_camera_list_gsds
export CM_CONFIG_FILE_PATH="/opt/usr/wx/newap2/cluster/gea/runtime_service/HwCameraList/etc/hw_camera_listProcess1"
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/opt/usr/wx/newap2/cluster/gea/lib:/opt/usr/root/usr/lib64/:/opt/usr/ap_adsfi/third_party
pmupload /opt/usr/wx/newap2/cluster/gea/runtime_service/HwCameraList/bin/HwCameraList  --allocGroup = default_dm
