#!/bin/bash
if [ ! -d "translations/" ];then
  mkdir translations
fi
cd ./translations
rm -f dock-network-plugin.ts
lupdate ../ ../../src -ts -no-obsolete dock-network-plugin.ts
cd ../

tx push -s -b m20
