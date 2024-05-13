#!/bin/bash

DESKTOP_SOURCE_FILE=org.deepin.dde.control-center.desktop
DESKTOP_TS_DIR=../translations/desktop/

/usr/bin/deepin-desktop-ts-convert desktop2ts $DESKTOP_SOURCE_FILE $DESKTOP_TS_DIR
