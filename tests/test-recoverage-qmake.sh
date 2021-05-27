#!/bin/bash
BUILD_DIR=build
REPORT_DIR=report
cd ../
rm -rf $BUILD_DIR
mkdir $BUILD_DIR
cd $BUILD_DIR
cmake ..
make -j16
make check

cd tests/

# dccwidgets
mv ./dccwidgets/asan.log* ./dccwidgets/asan_dccwidgets.log

#dde-control-center
mv ./dde-control-center/asan_bluetooth.log* ./dde-control-center/asan_bluetooth.log
mv ./dde-control-center/asan_datetime.log* ./dde-control-center/asan_datetime.log
mv ./dde-control-center/asan_defapp.log* ./dde-control-center/asan_defapp.log
mv ./dde-control-center/asan_mouse.log* ./dde-control-center/asan_mouse.log
mv ./dde-control-center/asan_notification.log* ./dde-control-center/asan_notification.log