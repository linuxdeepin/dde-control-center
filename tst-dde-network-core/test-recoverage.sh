#!/bin/bash

BUILD_DIR=build
REPORT_DIR=report

rm -rf $BUILD_DIR
mkdir $BUILD_DIR
cd $BUILD_DIR

cmake -DCMAKE_BUILD_TYPE=Debug ../../

cmake ../../
make -j 8

cd tst-dde-network-core/

lcov -c -i -d ./ -o init.info
./tst-dde-network --gtest_output=xml:dde_tst-dde-network-core.xml
lcov -c -d ./ -o cover.info
lcov -a init.info -a cover.info -o total.info
lcov -r total.info "*/tests/*" "*/usr/include*" "*build/src*" -o final.info

rm -rf ../../$REPORT_DIR
mkdir -p ../../$REPORT_DIR
genhtml -o ../../$REPORT_DIR final.info

mv asan.log* asan_dde-network-core.log
