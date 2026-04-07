#! /bin/sh
current_path="$(cd "$( dirname "$0")" && pwd )"
current_parent_path="$(dirname "${current_path}")"
parent_path="${current_parent_path}""/runtime_service/Control/etc/"
completion_txt=""
for dir in $(ls ${parent_path})
do
    real_path=${parent_path}/${dir}
    if [ -d ${real_path} ];then
        completion_txt="${completion_txt} ${dir}"
    fi
done

if [ ${#completion_txt} -gt 0 ]; then

    echo "Please select a process:"
    start=1
    for name in ${completion_txt}
    do
        echo "$start) $name"
        start=$((start + 1))
    done

    read -p "#? " answer
    if [ -z ${answer} ] || [ ${answer} -ge ${start} ]; then
        echo "Invalid input."
        exit
    fi
    var=`echo ${completion_txt} | awk -v pos="${answer}" -F " " '{print $pos}'`
fi
export CM_CONFIG_FILE_PATH=${current_parent_path}/runtime_service/Control/etc/${var}
# 日志控制台输出开关, 可选
export ENABLE_MDC_LOG_CONSOLE=ON

# 日志输出级别选项 可选择 FATAL, ERROR, WARN, INFO, DEBUG, VERBOSE
# export SET_MDC_AP_LOG_LEVEL=DEBUG

export LD_LIBRARY_PATH=${current_parent_path}/lib/:${LD_LIBRARY_PATH}
arch=$(uname -m)
if [ $arch == "x86_64" ]; then
    ${current_parent_path}/runtime_service/Control/bin/Control
else
    pmupload ${current_parent_path}/runtime_service/Control/bin/Control --allocGroup=default_dm
fi