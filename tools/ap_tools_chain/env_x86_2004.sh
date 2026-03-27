#!/bin/bash

export KTD_HOME="$( cd "$(dirname "${BASH_SOURCE[0]}")" && pwd )"
export KTD_MACHINE=x86_2004
export KTD_ROOT=$KTD_HOME/runtime/$KTD_MACHINE
export APP_ROOT=$KTD_HOME/runtime
echo "KTD_HOME = $KTD_HOME"

export LD_LIBRARY_PATH=$KTD_ROOT/lib/avos_communication_adaptor:$KTD_ROOT/lib:$LD_LIBRARY_PATH

export AVOS_DOMAIN=adsfi
cd $KTD_ROOT/bin
