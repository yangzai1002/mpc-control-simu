
#!/bin/bash

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/opt/usr/zxz/aarch64-linux-gnu
export BOARD_IP=26.28.1.2
export AVOS_DOMAIN=adsfi
# 配置文件路径
script_dir=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
TOPIC_REMAP_INFO="${script_dir}/../share/remap_info/topic_remap.opt"

# TOPIC_REMAP_INFO="/home/idriver/ap_adsfi/workspace/cmake/share/remap_info/topic_remap.opt"

# script_dir=$(cd " $(dirname '$0')"; pwd)

# script_dir=$(dirname "$(realpath '$0')")
echo "script_dir $script_dir"


AP_BAG() {

    # 检查参数数量
    if [ $# -lt 2 ]; then
        echo "Usage: AP_BAG record value1 value2 ..."
        return 1
    fi

    echo $@
    $script_dir/../tools/remap/aarch_exe_remap_cmd rtfbag $@ $TOPIC_REMAP_INFO

}
# 函数：打印所有 key 列表
AP_TOPIC_LIST() {
    if [[ ! -f "$TOPIC_REMAP_INFO" ]]; then
        echo "错误: 配置文件不存在"
        return 1
    fi

    echo "所有 Key 列表:"
    while IFS=',' read -r key value; do
        # 跳过空行和注释行
        [[ -z "$key" || "$key" =~ :space:]]*# ]] && continue
        echo "$key"
    done < "$TOPIC_REMAP_INFO"
}

# 函数：打印所有 value 列表
RTF_TOPIC_LIST() {
    if [[ ! -f "$TOPIC_REMAP_INFO" ]]; then
        echo "错误: 配置文件不存在"
        return 1
    fi

    echo "所有 Value 列表:"
    while IFS=',' read -r key value; do
        # 跳过空行和注释行
        [[ -z "$key" || "$key" =~ :space:]]*# ]] && continue
        echo "$value"
    done < "$TOPIC_REMAP_INFO"
}

# 函数：根据 key 查找对应的 value
AP_TOPIC_ECHO() {
    local search_key="$1"
    
    # 检查输入参数是否为空
    if [[ -z "$search_key" ]]; then
        echo "错误: 请输入有效的 key"
        return 1
    fi
    
    # 检查配置文件是否存在
    if [[ ! -f "$TOPIC_REMAP_INFO" ]]; then
        echo "错误: 配置文件不存在"
        return 1
    fi

    # 检查输入的 key 是否包含特殊字符 $1
    if [[ "$search_key" == *'$1'* ]]; then
        echo "警告: 输入的 key 包含特殊字符 '$1'"
    fi

    while IFS=',' read -r key value; do
        # 跳过空行和注释行
        [[ -z "$key" || "$key" =~ :space:]]*# ]] && continue
        if [[ "$key" == "$search_key" ]]; then
            echo "exe rtfevent echo $value"
            rtfevent echo $value
            return 0
        fi
    done < "$TOPIC_REMAP_INFO"

    echo "未找到键: $search_key"
    return 1
}


AP_TOPIC_HZ() {
    local search_key="$1"
    
    # 检查输入参数是否为空
    if [[ -z "$search_key" ]]; then
        echo "错误: 请输入有效的 key"
        return 1
    fi
    
    # 检查配置文件是否存在
    if [[ ! -f "$TOPIC_REMAP_INFO" ]]; then
        echo "错误: 配置文件不存在"
        return 1
    fi

    # 检查输入的 key 是否包含特殊字符 $1
    if [[ "$search_key" == *'$1'* ]]; then
        echo "警告: 输入的 key 包含特殊字符 '$1'"
    fi

    while IFS=',' read -r key value; do
        # 跳过空行和注释行
        [[ -z "$key" || "$key" =~ :space:]]*# ]] && continue
        if [[ "$key" == "$search_key" ]]; then
            echo "exe rtfevent hz $value"
            rtfevent hz $value
            return 0
        fi
    done < "$TOPIC_REMAP_INFO"

    echo "未找到键: $search_key"
    return 1
}



# AP_BAG_RECORD() {
#     local search_key="$1"
    
#     # 检查输入参数是否为空
#     if [[ -z "$search_key" ]]; then
#         echo "错误: 请输入有效的 key"
#         return 1
#     fi
    
#     # 检查配置文件是否存在
#     if [[ ! -f "$TOPIC_REMAP_INFO" ]]; then
#         echo "错误: 配置文件不存在"
#         return 1
#     fi

#     # 检查输入的 key 是否包含特殊字符 $1
#     if [[ "$search_key" == *'$1'* ]]; then
#         echo "警告: 输入的 key 包含特殊字符 '$1'"
#     fi

#     while IFS=',' read -r key value; do
#         # 跳过空行和注释行
#         [[ -z "$key" || "$key" =~ :space:]]*# ]] && continue
#         if [[ "$key" == "$search_key" ]]; then
#             echo "exe rtfevent hz $value"
#             rtfevent hz $value
#             return 0
#         fi
#     done < "$TOPIC_REMAP_INFO"

#     echo "未找到键: $search_key"
#     return 1
# }


# 示例调用（可选）
# AVOS_TOPIC_LIST
# print_values
# get_value_by_key "some_key"
