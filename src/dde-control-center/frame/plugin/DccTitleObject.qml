// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import org.deepin.dtk 1.0 as D
import "DccUtils.js" as DccUtils

DccObject {
    pageType: DccObject.Item
    page: ColumnLayout {
        DccLabel {
            Layout.fillWidth: true
            property D.Palette textColor: D.Palette {
                normal: Qt.rgba(0, 0, 0, 0.9)
                normalDark: Qt.rgba(1, 1, 1, 0.9)
            }
            font: DccUtils.copyFont(D.DTK.fontManager.t5, {
                                        "weight": 700
                                    })
            text: dccObj.displayName
            // color: D.ColorSelector.textColor
        }
        DccLabel {
            Layout.fillWidth: true
            visible: text !== ""
            font: D.DTK.fontManager.t8
            text: dccObj.description
            opacity: 0.5
        }
    }
    onParentItemChanged: {
        if (parentItem) {
            parentItem.leftPadding = 10
        }
    }
}
