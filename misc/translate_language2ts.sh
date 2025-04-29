#!/bin/bash
# 根据/usr/share/i18n/language_info.json生成keyboard_language_*.ts文件脚本
set -e
LUPDATE_BIN=/usr/lib/qt6/bin/lupdate
LANGUAGE_INFO_FILE=/usr/share/i18n/language_info.json

# 处理参数
if [ $# -gt 0 ]; then
  LANGUAGE_INFO_FILE=$1
fi

if [ ! -r "$LANGUAGE_INFO_FILE" ]; then
  echo "Error: file '$LANGUAGE_INFO_FILE' does not exist or is not readable." >&2
  exit 1
fi
# 组织头文件
SCRIPT_DIR=$(dirname "$0")
TMP_FILE=$(mktemp /tmp/dcc_language_info.XXXXXX.h)

echo 'namespace dcc {namespace keyboard {class Language {Q_OBJECT static const char *const language_info[] = {' >$TMP_FILE
grep Description $LANGUAGE_INFO_FILE >>$TMP_FILE
echo '};}}}' >>$TMP_FILE

sed -i -E 's/\s+"Description":\s+/QT_TR_NOOP(/g' $TMP_FILE
sed -i -E 's/,/),/g' $TMP_FILE

# 生成ts文件
TS_DIR=$SCRIPT_DIR/../translations/
$LUPDATE_BIN $TMP_FILE -ts -no-obsolete -no-ui-lines -locations none "$TS_DIR"keyboard_language_*.ts
# 清理缓存
rm -f $TMP_FILE
