export KTD_ROOT=/opt/usr/wx/newap2/
export PROJ_CFG_PATH=/opt/usr/wx/newap2/config
export MODEL_FILE_PATH="/opt/usr/wx/newap2/config/terrain_seg_param.xml"
export MAP_FUSION_FILE_PATH="/opt/usr/wx/newap2/config/mapfusion_param.xml"
export CM_CONFIG_FILE_PATH="/opt/usr/wx/newap2/cluster/gea/runtime_service/LidarTerrainMapDetection/etc/lidar_terrain_map_detectionProcess29/"
export ENABLE_MDC_LOG_CONSOLE=ON
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/opt/usr/wx/newap2/cluster/gea/lib:/opt/usr/root/usr/lib64/:/opt/usr/ap_adsfi/third_party
pmupload /opt/usr/wx/newap2/cluster/gea/runtime_service/LidarTerrainMapDetection/bin/LidarTerrainMapDetection  --allocGroup=default_dm