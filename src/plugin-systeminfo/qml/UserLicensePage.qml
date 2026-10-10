// SPDX-FileCopyrightText: 2024 - 2026 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
// import org.deepin.dtk 1.0 as D
import QtQuick 2.15
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15

import org.deepin.dtk 1.0
import org.deepin.dcc 1.0


DccObject {
    id: userLicensePage
    name: "content"
    parentName: "system/userLicense"
    pageType: DccObject.Item
    weight: 20
    page: Item {
        implicitHeight: contentLabel.implicitHeight
        Label {
            id: contentLabel
            anchors.fill: parent
            horizontalAlignment: Text.AlignLeft
            font: DTK.fontManager.t6
            text: dccData.systemInfoMode().userLicense
            wrapMode: Text.WordWrap
            textFormat: dccData.systemInfoMode().userLicenseFormat === Qt.MarkdownText ? Text.MarkdownText : Text.PlainText
            onLinkActivated: (link) => Qt.openUrlExternally(link)

            HoverHandler {
                cursorShape: parent.hoveredLink ? Qt.PointingHandCursor : Qt.ArrowCursor
            }
        }
        Image {
            anchors.right: parent.right
            anchors.top: parent.top
            width: 44
            height: 20
            sourceSize.width: 44
            sourceSize.height: 20
            opacity: 0.6
            visible: DccApp.isCommunitySystem()
            source: "qrc:/icons/deepin/builtin/icons/dcc_deepin_logo.svg"
            fillMode: Image.PreserveAspectFit
        }
    }
}

