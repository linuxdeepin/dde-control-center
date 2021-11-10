#!/bin/bash
cd ./translations
rm -f dde-control-center_en.ts
lupdate ../src/ -ts -no-obsolete dde-control-center_en.ts
sed -e 's/DCC_NAMESPACE/dccV20/g' dde-control-center_en.ts > tmp.ts
mv tmp.ts dde-control-center_en.ts
cd ../

tx push -s -b maintain
