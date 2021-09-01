#!/bin/bash
BUILD_DIR=build
REPORT_DIR=report
cd ../
rm -rf $BUILD_DIR
mkdir $BUILD_DIR
cd $BUILD_DIR
cmake -DCMAKE_BUILD_TYPE=Debug ..
make -j16

cd tests/

cd dccwidgets/
./dccwidgets-unittest --gtest_output=xml:dde_test_report_dccwidgets.xml
# dccwidgets
mv asan.log* asan_dccwidgets.log

cd ..
cd dde-control-center/
echo " =================== CREAT LCOV REPROT BEGIN ==================== "
#lcov --directory ./CMakeFiles/bluetooth-unittest.dir --zerocounters
lcov --directory ./CMakeFiles/mouse-unittest.dir --zerocounters
lcov --directory ./CMakeFiles/datetime-unittest.dir --zerocounters
lcov --directory ./CMakeFiles/notification-unittest.dir --zerocounters
lcov --directory ./CMakeFiles/defapp-unittest.dir --zerocounters
lcov --directory ./CMakeFiles/systeminfo-unittest.dir --zerocounters
lcov --directory ./CMakeFiles/keyboard-unittest.dir --zerocounters
echo " =================== Start Unit  ==================== "
#./bluetooth-unittest --gtest_output=xml:dde_test.xml
./mouse-unittest --gtest_output=xml:dde_test_report_mouse.xml
./systeminfo-unittest --gtest_output=xml:dde_test_report_systeminfo.xml
./datetime-unittest --gtest_output=xml:dde_test_report_datetime.xml
./defapp-unittest --gtest_output=xml:dde_test_report_defapp.xml
./notification-unittest --gtest_output=xml:dde_test_report_notification.xml
./keyboard-unittest --gtest_output=xml:dde_test_report_keyboard.xml
echo " =================== do filter begin ==================== "
lcov --directory . --capture --output-file ./coverage.info
echo " =================== get info end ==================== "
echo " =================== do filter begin ==================== "

lcov --remove ./coverage.info  '*/${MOUSE_NAME}_autogen/*' '*/${NOTIFICATION_NAME}_autogen/*' '*/${DATETIME_NAME}_autogen/*' '*/${DEFAPP_NAME}_autogen/*' '*/${SYSTEMINFO_NAME}_autogen/*' '*/${KEYBOARD_NAME}_autogen/*' '*/usr/include/*' '*/tests/*' '*/display/*' '*/googletest/*' '*/widgets/*' '*/interface/*' -o ./coverage.info
echo " =================== do filter end ==================== "
genhtml -o ../../tests/report ./coverage.info
echo " =================== CREAT LCOV REPROT END ==================== "
echo " Coverage files have been output to ${CMAKE_BINARY_DIR}/tests/report "

#dde-control-center
#mv asan_bluetooth.log* asan_bluetooth.log
mv asan_mouse.log* asan_mouse.log
mv asan_systeminfo.log* asan_systeminfo.log
mv asan_defapp.log* asan_defapp.log
mv asan_datetime.log* .asan_datetime.log
mv asan_notification.log* asan_notification.log
mv asan_keyboard.log* asan_keyboard.log