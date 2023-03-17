#!/bin/bash

# 根据CMakeLists.txt更新ts文件
# 参数1 ts文件名，不带_en.ts
# 参数2 代码路径
# 参数3 附加头文件路径，可无
function lupdateProject() {
	local INC_FLAG=
	local SRC_DIR=$2
	local TS_FILE=$1_en.ts
	echo 工程:$1
        echo 代码路径:$2
	if [ $# == 3 ]; then
		echo 附加头文件路径:$3
		INC_FLAG=-I\ $3
	fi
        cd translations
	rm -f $TS_FILE

        lupdate $SRC_DIR $INC_FLAG -no-obsolete -no-ui-lines -locations none -ts $TS_FILE

        sed -e 's/DCC_NAMESPACE/dccV23/g' $TS_FILE >tmp.ts
	mv tmp.ts $TS_FILE
        cd ..
}

CURRENT_DIR=$(pwd)
PROJECT_DIR=$(dirname $(realpath $(dirname $(realpath $0))))
cd ${PROJECT_DIR}
echo 当前目录：${CURRENT_DIR}
echo 工程目录：${PROJECT_DIR}
# 执行目录会切换到工程目录

# dde-control-center_en.ts
lupdateProject dde-control-center ${PROJECT_DIR}/src ${PROJECT_DIR}/include

# desktop.ts  无源文件
# exec ./misc/translate_desktop2ts.sh
DESKTOP_SOURCE_FILE=misc/dde-control-center.desktop
DESKTOP_TS_DIR=translations/desktop/
/usr/bin/deepin-desktop-ts-convert desktop2ts $DESKTOP_SOURCE_FILE $DESKTOP_TS_DIR

# keyboard_language_en.ts
# keyboard_language 无源文件，直接修改keyboard_language_en.ts

# 推送翻译
tx push -s --branch m23
cd ${CURRENT_DIR}
