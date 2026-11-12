#!/bin/bash

set -e

DESKTOP_OUTPUT_FILE=${1:?Usage: translate_ts2desktop.sh OUTPUT_FILE}
DESKTOP_SOURCE_FILE=misc/org.deepin.dde.control-center.desktop

DESKTOP_TS_DIR=translations/desktop/

/usr/bin/deepin-desktop-ts-convert ts2desktop "$DESKTOP_SOURCE_FILE" "$DESKTOP_TS_DIR" "$DESKTOP_OUTPUT_FILE"
