// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
// import QtQuick 2.15

function copyFont(srcFont, propertys) {
    return Qt.font({
        "family" : propertys.hasOwnProperty("family") ? propertys.family : srcFont.family,
        "bold" : propertys.hasOwnProperty("bold") ? propertys.bold : srcFont.bold,
        "italic" : propertys.hasOwnProperty("italic") ? propertys.italic : srcFont.italic,
        "underline" : propertys.hasOwnProperty("underline") ? propertys.underline : srcFont.underline,
        "pointSize" : propertys.hasOwnProperty("pointSize") ? propertys.pointSize : srcFont.pointSize,
        "pixelSize" : propertys.hasOwnProperty("pixelSize") ? propertys.pixelSize : srcFont.pixelSize
    })
}
