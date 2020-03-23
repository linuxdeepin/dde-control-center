rm translations/dde-control-center_en_US.ts
lupdate ./src/ -ts translations/dde-control-center_en_US.ts
sed -i 's/DCC_NAMESPACE/dccV20/g' translations/dde-control-center_en_US.ts
