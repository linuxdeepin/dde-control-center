#!/bin/bash
cd ./translations
rm -f dde-control-center_en.ts
# files.tmp 参加翻译的文件列表，从CMakeLists中提取.h和.cpp文件
rm -f files.tmp
for i in $(grep "\.h" ../src/frame/CMakeLists.txt)
do
	echo ../src/frame/$i >> files.tmp
done

for i in $(grep "\.cpp" ../src/frame/CMakeLists.txt)
do
	echo ../src/frame/$i >> files.tmp
done

lupdate @files.tmp -ts -no-obsolete dde-control-center_en.ts
rm -f files.tmp

sed -e 's/DCC_NAMESPACE/dccV20/g' dde-control-center_en.ts > tmp.ts
mv tmp.ts dde-control-center_en.ts
cd ../

tx push -s -b m20
