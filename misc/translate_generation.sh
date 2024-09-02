#!/bin/bash
# this file is used to auto-generate .qm file from .ts file.
# author: shibowen at linuxdeepin.com
export PATH=/usr/lib/qt6/bin:$PATH
cd translations
ts_list=(`ls *.ts`)
OUT_DIR=${1}
echo OUT_DIR： ${OUT_DIR}
mkdir -p ${OUT_DIR}
for ts in "${ts_list[@]}"
do
    printf "\nprocess ${ts}\n"
    lrelease "${ts}" -qm ${OUT_DIR}/${ts:0:-2}qm
done
