#!/bin/bash
if [ ! -d "translations/" ];then
  mkdir translations
fi
cd ./translations
rm -f dde-network-dialog.ts
lupdate ../ ../../src -ts -no-ui-lines -locations none -no-obsolete dde-network-dialog.ts
cd ../

# tx push -s -b m20
