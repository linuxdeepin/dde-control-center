#!/bin/bash
cp ".transifexrc" ${HOME}/
cd ./translations
rm -f dde-control-center_en_US.ts
lupdate ../src/ -ts dde-control-center_en_US.ts
sed -e 's/DCC_NAMESPACE/dccV20/g' dde-control-center_en_US.ts > tmp.ts
mv tmp.ts dde-control-center_en_US.ts
cd ../
tx push -s -b klu
