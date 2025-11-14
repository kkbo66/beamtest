#!/bin/bash
para_num() {
    if test $# -lt 1; then
        echo "Please Input data file realpath, or plus 2nd parameter: file number to process"
        exit 1
    fi
}
get_last_dirs() {
    local path="$1"
    local n="$2"
    IFS='/' read -r -a parts <<<"$path"
    local total=${#parts[@]}
    for ((i = total - n; i < total; i++)); do
        eval "dir$((i - total + n + 1))=${parts[$i]}"
    done
}
# 输入参数1: UI01服务器中的数据文件夹绝对路径“**/ECAL/”
# 输入参数2：处理的文件数目(缺省时拷贝全部文件)
DATAFILEPATH=$1
FILENUMBER=$2
para_num $@

DATAFILEPATH=${DATAFILEPATH%/}
if [ $(basename $DATAFILEPATH) != "ECAL" ]; then
    DATAFILEPATH=$DATAFILEPATH/ECAL
fi
# 获取最后四级目录名到dir1、2、3、4
get_last_dirs $DATAFILEPATH 4
# 脚本所在路径
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
DATA_DIR=${SCRIPT_DIR}/data/"$dir1$dir2$dir3"
mkdir -p $DATA_DIR
echo "make data directory $(realpath $DATA_DIR)"
pushd ${DATA_DIR} >/dev/null

# 数据处理程序路径
DECODE_DIR=${SCRIPT_DIR}/../../build

# 解码文件生成decode.root
if [ ! -f "decode.root" ]; then
    # 判断是否生成解码程序
    if [ ! -f "${DECODE_DIR}/ECALdig2root2025" ]; then
        echo "Decode programe dose not exist!"
        exit 1
    fi
    # 解码前FILENUMBER个文件
    if [ -n "$FILENUMBER" ]; then
        ls $DATAFILEPATH/data_ECAL*.dat | sort | head -n "$FILENUMBER" | while read -r f; do
            datapath=$(realpath "$f")
            ${DECODE_DIR}/ECALdig2root2025 ${datapath} "$(basename "${datapath%.dat}").root"
        done
    else
        ls $DATAFILEPATH/data_ECAL*.dat | sort | while read -r f; do
            datapath=$(realpath "$f")
            ${DECODE_DIR}/ECALdig2root2025 ${datapath} "$(basename "${datapath%.dat}").root"
        done
    fi
    # 等待后台程序执行完毕
    # wait
    hadd decode.root data*ECAL*.root
    echo "generate $(pwd)/decode.root"
    # 移除中间数据文件
    rm data*ECAL*
fi

# 数字化和重建
if [ ! -f 'digi.root' ]; then
    ${DECODE_DIR}/ECALDigi $(pwd)/decode.root digi.root
    echo "generate $(pwd)/digi.root"
fi
if [ ! -f "rec.root" ]; then
    ${DECODE_DIR}/Reconstruction $(pwd)/digi.root rec.root
    echo "generate $(pwd)/rec.root"
fi

popd >/dev/null
