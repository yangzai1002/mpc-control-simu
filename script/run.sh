#!/bin/bash
#update history:
#2018-05-31   add so libary solution     xy
dir_=$(cd `dirname $0`; pwd)

function ListNode() {
  
  if [ $# -eq 1 ]; then
    echo "ListNode $1"
    ./adsfi_demo_gen nodelist $1
  else
    echo "参数错误,请输入路径"
  fi
}


function GenDemo() {
  
  if [ $# -eq 1 ]; then
    echo "ListNode $1"
    ./adsfi_demo_gen fillallstop $1
    sync
    ./adsfi_demo_gen fillallinit $1
    sync
    ./adsfi_demo_gen fillallpub $1
    sync
    ./adsfi_demo_gen fillallsub $1
    sync
    ./adsfi_demo_gen fillcmake $1
    sync
    ./adsfi_demo_gen fillallinterface $1
    sync
    ./adsfi_demo_gen fillallheader $1
    sync
     ./adsfi_manifest_parse $dir_/../workspace/cmake/share/em_sm_cfg/process_config.json $1/../outputcfg/Host0DpMachine  1
    sync
  else
    echo "参数错误,请输入路径"
  fi
}



function print_usage() {
  RED='\033[0;31m'
  BLUE='\033[0;34m'
  BOLD='\033[1m'
  NONE='\033[0m'

  echo -e "\n${RED}Usage${NONE}:
  ${BOLD}bash run.sh${NONE} [OPTION]"

  echo -e "\n${RED}Options${NONE}:
  ${BLUE}list path${NONE}: list all node in path
  ${BLUE}gen${NONE}: fill all demo in path
  "
}

function main() {
  # CheckVersion
  dir_=$(cd `dirname $0`; pwd)
  cd $dir_


  local cmd=$1
  shift

  case $cmd in
    list)
      ListNode $@
      ;;
    gen)
      GenDemo $@
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
