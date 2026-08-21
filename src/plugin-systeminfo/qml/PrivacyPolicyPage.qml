// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
// import org.deepin.dtk 1.0 as D
import QtQuick 2.15
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15

import org.deepin.dtk 1.0
import org.deepin.dcc 1.0
//import org.deepin.dcc.systemInfo 1.0

DccObject {

    DccObject {
        name: "title"
        parentName: "system/privacyPolicy"
        pageType: DccObject.Item
        weight: 20
        page: Label {
            Layout.alignment: Qt.AlignHCenter
            horizontalAlignment: Text.AlignHCenter
            font: DTK.fontManager.t4
            text: qsTr("Privacy Policy")
        }
    }
    DccObject {
        name: "content"
        parentName: "system/privacyPolicy"
        pageType: DccObject.Item
        weight: 30
        page:
            Label {
                id: policyLabel
                textFormat: Text.RichText
                font: DTK.fontManager.t6
                horizontalAlignment: Text.AlignLeft
                text: dccData.systemInfoMode().privacyPolicy
                wrapMode: Text.WordWrap
                opacity: 0.7

                property string currentLinkUrl: ""

                // 超链接点击事件
                onLinkActivated: function(url) {
                    console.log("点击的链接是: " + url)
                    Qt.openUrlExternally(url) // 使用默认浏览器打开链接
                }

                MouseArea {
                    id: labelMouseArea
                    anchors.fill: parent
                    acceptedButtons: Qt.LeftButton | Qt.RightButton
                    hoverEnabled: true

                    cursorShape: policyLabel.linkAt(mouseX, mouseY) ? Qt.PointingHandCursor : Qt.ArrowCursor // Change cursor over links

                    onPressed: (mouse)=> {
                        if (mouse.button === Qt.RightButton) {
                            var link = policyLabel.linkAt(mouse.x, mouse.y)
                            if (link) {
                                console.log("Right-clicked on link: " + link)
                                policyLabel.currentLinkUrl = link
                                contextMenu.popup()
                            }
                            mouse.accepted = true
                        } else {
                            mouse.accepted = false
                        }
                    }
                }

                Menu {
                    id: contextMenu
                    MenuItem {
                        id: copyLinkMenuItem
                        text: qsTr("Copy Link Address") + "(L)"
                        onTriggered: {
                            console.log("Copying link: " + policyLabel.currentLinkUrl)
                            dccData.systemInfoWork().copyTextToClipboard(policyLabel.currentLinkUrl)
                        }
                    }
                    Shortcut {
                         sequence: "L"
                         onActivated: {
                             console.log("Shortcut L activated to select copy link")
                             if (contextMenu.visible) {
                                 contextMenu.currentIndex = 0
                                 contextMenu.focus = true
                             }
                         }
                     }
                }
            }
    }
}
