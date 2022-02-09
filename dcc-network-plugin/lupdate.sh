#!/bin/bash
if [ ! -d "translations/" ];then
  mkdir translations
fi
cd ./translations
rm -f dcc-network-plugin.ts
lupdate ../ ../../src -ts -no-ui-lines -locations none -no-obsolete dcc-network-plugin.ts
sed -e 's/DCC_NAMESPACE/dccV20/g' dcc-network-plugin.ts > tmp.ts
mv tmp.ts dcc-network-plugin.ts
cd ../
tx push -s -b m20
