// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import QtQuick.Controls 2.3
import Qt.labs.qmlmodels 1.2
import org.deepin.dtk 1.0 as D
import org.deepin.dtk.style 1.0 as DS
import org.deepin.dcc 1.0

FocusScope {
    id: root
    property alias model: repeater.model
    signal clicked(var model)

    activeFocusOnTab: repeater.count > 2

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

        function nextItemFocus(forward) {
            var focusIndex = -1
            for (var i = 0; i < repeater.count; i++) {
                var item = repeater.itemAt(i)
                if (item.focus) {
                    focusIndex = i
                    break
                }
            }
            focusIndex = forward ? focusIndex - 2 : focusIndex + 2
            if (focusIndex >= 0 && focusIndex < repeater.count - 1) {
                repeater.itemAt(focusIndex).forceActiveFocus()
            }
        }

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
                    elide: Text.ElideLeft
                    focus: true
                    Loader {
                        anchors.fill: parent
                        active: parent.activeFocus
                        sourceComponent: D.FocusBoxBorder {
                            radius: DS.Style.control.radius
                            color: parent.palette.highlight
                        }
                    }
                    Keys.onPressed: event => {
                                        switch (event.key) {
                                            case Qt.Key_Space:
                                            case Qt.Key_Enter:
                                            case Qt.Key_Return:
                                            root.clicked(model)
                                            break
                                            case Qt.Key_Left:
                                            case Qt.Key_Up:
                                            repeater.nextItemFocus(true)
                                            break
                                            case Qt.Key_Right:
                                            case Qt.Key_Down:
                                            repeater.nextItemFocus(false)
                                            break
                                        }
                                    }

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
                    onImplicitWidthChanged: root.updateLayout()
                    onImplicitHeightChanged: root.updateLayout()
                }
            }
        }
    }
}
