#!/bin/bash
# 这个脚本是对UI服务器上的束流数据进行批量处理
# 传入参数是文件列表的绝对路径，文件每一行是输入参数，如：/ustcfs3/stcf/BT2025/2ECAL_PIDE_PIDB/e+/1.0GeV/1024run07/ 0 10

# 检查是否传入文件路径
if [[ $# -lt 1 ]]; then
    echo "输入文件列表路径"
    exit 1
fi

input_file="$1"

# 检查输入文件是否存在
if [[ ! -f "$input_file" ]]; then
    echo "错误：文件不存在：$input_file"
    exit 1
fi

# 脚本所在路径
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
# 处理脚本
PROCESS_SCRIPT=$SCRIPT_DIR/processUI.sh

# 判断处理脚本是否存在
if [ ! -f $PROCESS_SCRIPT ]; then
    echo "processUI.sh does not exist!"
    exit 1
fi

# 逐行读取文件
while read -r FILEPATH RUNSORT FILENUMBER; do
    [[ -z "$FILEPATH" ]] && continue
    echo "FILEPATH='$FILEPATH' RUNSORT='$RUNSORT' FILENUMBER='$FILENUMBER'"
    "$PROCESS_SCRIPT" "$FILEPATH" "$RUNSORT" "$FILENUMBER"
done <"$input_file"
