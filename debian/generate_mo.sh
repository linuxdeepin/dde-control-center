#!/bin/sh

if [ ! -d mo ];then
    mkdir mo
fi

for i in `ls | grep .po$`;do
    if [ ! -d mo/${i%%.po}/LC_MESSAGES ];then
        mkdir -p mo/${i%%.po}/LC_MESSAGES
    fi
    msgfmt -o mo/${i%%.po}/LC_MESSAGES/dde-control-center.mo ${i}
done
