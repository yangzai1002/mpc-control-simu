#!/bin/bash
#update history:
#2026-01-06   add so libary solution     xy

dir_=$(cd `dirname $0`; pwd)
export ap_adsfi=$dir_/../
export HOST_IP=26.28.1.2
export HOST_USERNAME=root
export HOST_PASSWD=Huawei12#$

# export HOST_IP=192.168.1.240
# export HOST_USERNAME=idriver
# export HOST_PASSWD=idriver
export MDS_PROJECT=$dir_/..//tool_chain/mds_wks/MDS_MDC2501MidTracked




function print_usage() {
  RED='\033[0;31m'
  BLUE='\033[0;34m'
  BOLD='\033[1m'
  NONE='\033[0m'

  echo -e "\n${RED}Usage${NONE}:
  ${BOLD}bash download.sh${NONE} [OPTION]"

  echo -e "\n${RED}Options${NONE}:
  ${BLUE}-d ${NONE}: download all path
  ${BLUE}-nd node_name${NONE}: download one node
  ${BLUE}-t ${NONE}: tar all path
  ${BLUE}-nt node_name${NONE}: tar one node
  ${BLUE}-m ${NONE}: download the msg
  "
}



function DownLoadMsg() {
  
  sshpass -p $HOST_PASSWD ssh ${HOST_USERNAME}@${HOST_IP} -t "mount -o remount,rw / && cp -r /opt/platform/mdc_platform/conf/rtftools/msg /opt/platform/mdc_platform/conf/rtftools/msg_src_bak"
  sshpass -p $HOST_PASSWD scp -r ${MDS_PROJECT}/outputcfg/DataType/* ${HOST_USERNAME}@${HOST_IP}:/opt/platform/mdc_platform/conf/rtftools/msg/
  sshpass -p $HOST_PASSWD ssh ${HOST_USERNAME}@${HOST_IP} -t "sync;sync"
  echo "DownLoadMsg over"
}

function DownLoadTools() {
  
  sshpass -p $HOST_PASSWD ssh ${HOST_USERNAME}@${HOST_IP} -t "mount -o remount,rw /"
  sshpass -p $HOST_PASSWD scp -r ${dir_}/../tools ${HOST_USERNAME}@${HOST_IP}:/opt/usr/ap_adsfi/
  echo "sshpass -p $HOST_PASSWD scp -r ${dir_}/../tools ${HOST_USERNAME}@${HOST_IP}:/opt/usr/ap_adsfi/"
  sshpass -p $HOST_PASSWD ssh ${HOST_USERNAME}@${HOST_IP} -t "sync;sync"
  echo "DownLoadTools over"
}






function main() {
  # CheckVersion
  dir_=$(cd `dirname $0`; pwd)
  cd $dir_


  local cmd=$1
  shift

  case $cmd in
    -m)
      DownLoadMsg 
      ;;
    -d)
      ./file_download -d
      DownLoadTools
      ;;
    -nd)
      ./file_download -nd $@
      ;;
    -t)
      ./file_download -t
      ;;
    -nt)
      ./file_download -nt $@
      ;;
    usage)
      print_usage
      ;;
    *)
      print_usage
      ;;
  esac
}

main $@
