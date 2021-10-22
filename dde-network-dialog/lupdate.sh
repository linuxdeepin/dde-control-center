#!/bin/bash
if [ ! -d "translations/" ];then
  mkdir translations
fi
cd ./translations
rm -f dock-network-dialog.ts
lupdate ../ ../../src -ts -no-obsolete dock-network-dialog.ts
cd ../

tx push -s -b m20
