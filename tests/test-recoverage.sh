#!/bin/bash

BUILD_DIR=build-ut
HTML_DIR=html
REPORT_DIR=report

cd ../
rm -rf $BUILD_DIR
mkdir $BUILD_DIR
cd $BUILD_DIR
mkdir $HTML_DIR
mkdir $REPORT_DIR


cmake -DCMAKE_BUILD_TYPE=Debug ../
make -j 8

cd tests/

lcov -c -i -d ./ -o init.info
./tst-dde-network --gtest_output=xml:../$REPORT_DIR/ut-report_dde-network-core.xml
lcov -c -d ./ -o cover.info
lcov -a init.info -a cover.info -o total.info
lcov -r total.info ./te"*/tests/*" "*/usr/include*" "*build/src*" -o final.info

genhtml -o ../$HTML_DIR final.info
mv ../$HTML_DIR/index.html ../$HTML_DIR/cov_dde-network-core.html

mv asan.log* ../asan_dde-network-core.log
