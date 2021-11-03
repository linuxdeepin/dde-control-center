#!/bin/bash
if [ ! -d "translations/" ];then
  mkdir translations
fi
cd ./translations
rm -f shell-network-plugin.ts
lupdate ../ ../../src ../../common-plugin -ts -no-ui-lines -locations none -no-obsolete shell-network-plugin.ts
cd ../

tx push -s -b m20
