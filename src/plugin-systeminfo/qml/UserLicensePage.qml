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
            height: 20
            property bool useDeepinWordmark: !["zh_CN", "zh_HK", "zh_TW"].includes(Qt.locale().name)
            width: useDeepinWordmark ? 67 : 44
            opacity: 0.6
            visible: DccApp.isCommunitySystem()
            // 中文（简/繁/港）环境用"深度"logo，其他语言用 deepin 字标
            source: useDeepinWordmark
                    ? "qrc:/icons/deepin/builtin/icons/dcc_deepin_wordmark.svg"
                    : "qrc:/icons/deepin/builtin/icons/dcc_deepin_logo.svg"
            fillMode: Image.PreserveAspectFit
        }
    }
}

