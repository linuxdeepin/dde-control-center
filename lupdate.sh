#!/bin/bash

# 根据CMakeLists.txt更新ts文件
# 参数1 ts文件名，不带_en.ts
# 参数2 CMakeLists.txt相对路径
# 参数3 附加头文件路径，可无
function lupdateProject() {
	local INC_FLAG=
	local SRC_DIR=$2
	local TS_FILE=$1_en.ts
	echo 工程:$1
	echo CMakeLists.txt路径:$2
	if [ $# == 3 ];then
		echo 附加头文件路径:$3
		INC_FLAG=-I\ $3
	fi
	cd ./translations
	rm -f $TS_FILE
	rm -f files.tmp
	for i in $(grep "\.h" $SRC_DIR/CMakeLists.txt)
	do
		echo $SRC_DIR/$i >> files.tmp
	done

	for i in $(grep "\.cpp" $SRC_DIR/CMakeLists.txt)
	do
		echo $SRC_DIR/$i >> files.tmp
	done
	lupdate @files.tmp $INC_FLAG -ts -no-obsolete $TS_FILE
	rm -f files.tmp

	sed -e 's/DCC_NAMESPACE/dccV20/g' $TS_FILE > tmp.ts
	mv tmp.ts $TS_FILE
	cd ../
}



# dde-control-center_en.ts
lupdateProject dde-control-center ../src/frame ../include

# dialogs_en.ts
lupdateProject dialogs ../src/reboot-reminder-dialog

# desktop.ts  无源文件

# recovery_en.ts
lupdateProject recovery ../abrecovery

# keyboard_language_en.ts
# keyboard_language 无源文件，直接修改keyboard_language_en.ts

# reset-pasword-dialog
lupdateProject reset-password-dialog ../src/reset-password-dialog

tx push -s -b m20
