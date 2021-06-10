#!/bin/bash
BUILD_DIR=build
REPORT_DIR=report
cd ../
rm -rf $BUILD_DIR
mkdir $BUILD_DIR
cd $BUILD_DIR
cmake -DCMAKE_BUILD_TYPE=Debug ..
make -j16
make check

cd tests/

cd dccwidgets/
./dccwidgets-unittest
# dccwidgets
mv asan.log* asan_dccwidgets.log

cd ..
cd dde-control-center/
./mouse-unittest
./systeminfo-unittest
./bluetooth-unittest
./datetime-unittest
./defapp-unittest
./notification-unittest

#dde-control-center
mv asan_bluetooth.log* asan_bluetooth.log
mv asan_mouse.log* asan_mouse.log
mv asan_systeminfo.log* asan_systeminfo.log
mv asan_defapp.log* asan_defapp.log
mv asan_datetime.log* .asan_datetime.log
mv asan_notification.log* asan_notification.log