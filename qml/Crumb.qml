// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import QtQuick.Controls 2.3
import Qt.labs.qmlmodels 1.2

import org.deepin.dcc 1.0

Rectangle {
    id: root
    property alias model: repeater.model
    signal clicked(var model)

    color: "transparent"
    clip: true

    Item {
        id: layoutView
        anchors.fill: parent
        function updateLayout() {
            let totalWidth = width
            let i
            let child
            for (i = children.length - 1; i >= 0; --i) {
                child = children[i]
                if (totalWidth <= 0) {
                    child.visible = false
                } else {
                    totalWidth -= child.implicitWidth
                    child.visible = true
                    if (totalWidth < 0) {
                        child.width = child.implicitWidth + totalWidth
                    } else {
                        child.width = child.implicitWidth
                    }
                }
            }

            let x = 0
            for (i = 0; i < children.length; ++i) {
                child = children[i]
                if (child.visible) {
                    child.x = x
                    child.y = (height - child.height) / 2
                    x += child.width
                }
            }
        }
        onWidthChanged: updateLayout()

        Repeater {
            id: repeater

            delegate: DelegateChooser {
                role: "type"
                DelegateChoice {
                    roleValue: 0
                    delegate: DccLabel {
                        text: " / "
                        clip: true
                        elide: Text.ElideLeft
                        color: palette.placeholderText
                    }
                }
                DelegateChoice {
                    roleValue: 1
                    delegate: DccLabel {
                        text: model.display
                        color: hovered ? palette.link : palette.text
                        clip: true
                        elide: Text.ElideLeft
                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                root.clicked(model)
                            }
                        }
                    }
                }
                DelegateChoice {
                    roleValue: 2
                    delegate: DccLabel {
                        text: model.display
                        clip: true
                        elide: Text.ElideLeft
                        color: "#78A6EB" // #78A6EB  "#0058DE"  "steelblue" "deepskyblue" darkturquoise
                        onImplicitWidthChanged: layoutView.updateLayout()
                        onImplicitHeightChanged: layoutView.updateLayout()
                    }
                }
            }
        }
    }
}
