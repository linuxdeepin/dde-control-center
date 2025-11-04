// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
.pragma library

function copyFont(srcFont, propertys) {
    return Qt.font({
                       "family": propertys.hasOwnProperty("family") ? propertys.family : srcFont.family,
                       "bold": propertys.hasOwnProperty("bold") ? propertys.bold : srcFont.bold,
                       "weight": propertys.hasOwnProperty("weight") ? propertys.weight : propertys.hasOwnProperty("bold") && propertys.bold ? 700 : srcFont.weight,
                       "italic": propertys.hasOwnProperty("italic") ? propertys.italic : srcFont.italic,
                       "underline": propertys.hasOwnProperty("underline") ? propertys.underline : srcFont.underline,
                       "pointSize": propertys.hasOwnProperty("pointSize") ? propertys.pointSize : srcFont.pointSize,
                       "pixelSize": propertys.hasOwnProperty("pixelSize") ? propertys.pixelSize : srcFont.pixelSize
                   })
}

function getMargin(w) {
    if (w > 1300) {
        return Math.round((w - 1200) / 2)
    }
    if (w >= 600) {
        return 50
    }
    return Math.round((w - 500) / 2)
}
