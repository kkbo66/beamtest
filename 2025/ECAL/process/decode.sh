#!/bin/bash
para_num(){
  if test $# -lt 1; then
    echo "Please Input data file realpath(/data/DAQ/Beam_2510/*.dat), or plus 2nd parameter: file number to copy"
    exit 1
  fi
}
para_num $@

# 输入参数: DAQ服务器中的数据文件夹路径
DATAFILEPATH=$1
FILENUMBER=$2

# 数据文件夹（日期）名
DATENAME=`basename $DATAFILEPATH`

# 脚本所在路径
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

mkdir -p ${SCRIPT_DIR}/data/${DATENAME}
pushd ${SCRIPT_DIR}/data/${DATENAME}>/dev/null

# 数据处理程序路径
DECODE_DIR=${SCRIPT_DIR}/../../build

# 解码生成总的文件total.root
if [ ! -f "total.root" ]; then
# 判断是否生成解码程序
if [ ! -f "${DECODE_DIR}/ECALdig2root2025" ]; then
echo "Decode programe dose not exist!"
exit 1
fi
i=0
# 拷贝DAQ服务器的数据到本地，跳过已有文件
ssh "lunon@192.168.7.150" "find '$DATAFILEPATH' -type f" | 
while read -r temp; do 
if [[ "$(basename $temp)" == data*ECAL*.dat ]]; then
# 后台拷贝文件和解码
dataname=$(basename $temp)
rsync -h --ignore-existing --progress  lunon@192.168.7.150:${DATAFILEPATH}/${dataname} .  
${DECODE_DIR}/ECALdig2root2025 ${dataname}  ${dataname%.dat}.root
cp ${DATAFILEPATH}/${dataname} .  
${DECODE_DIR}/ECALdig2root2025 ${dataname}  ${dataname%.dat}.root
((i++))
if [ $i -eq $FILENUMBER ]; then
break;
fi
fi
done
# 等待后台程序执行完毕
# wait
hadd total.root data*ECAL*.root 
# 移除中间数据文件
rm data*ECAL*
fi

# 拟合MIP峰位
# ${SCRIPT_DIR}/FitMip total.root

# Draw graphs
# /home/wangjunhao/beamtest/draw/drawval/build/drawval total.root

popd>/dev/null