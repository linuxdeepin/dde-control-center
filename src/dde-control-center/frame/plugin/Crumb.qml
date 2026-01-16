// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import QtQuick.Controls 2.3
import QtQml.Models //Delegatechoice for Qt >= 6.9
import Qt.labs.qmlmodels //DelegateChooser
import QtQuick.Layouts 1.15
import org.deepin.dtk 1.0 as D
import org.deepin.dtk.style 1.0 as DS
import org.deepin.dcc 1.0

FocusScope {
    id: root
    property alias model: repeater.model
    signal clicked(var model)

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
                x += child.width
            }
        }
    }
    activeFocusOnTab: repeater.count > 1
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
            focusIndex = forward ? focusIndex - 1 : focusIndex + 1
            if (focusIndex >= 0 && focusIndex < repeater.count - 1) {
                repeater.itemAt(focusIndex).forceActiveFocus()
            }
        }

        delegate: DelegateChooser {
            role: "type"
            DelegateChoice {
                roleValue: 1
                delegate: Control {
                    property alias pressed: ma.pressed
                    property D.Palette textColor: DS.Style.button.text
                    palette.windowText: D.ColorSelector.textColor
                    anchors.verticalCenter: parent.verticalCenter
                    bottomPadding: 3
                    topPadding: 3
                    leftPadding: 4
                    rightPadding: 4
                    focus: true
                    hoverEnabled: enabled
                    onImplicitWidthChanged: root.updateLayout()
                    contentItem: RowLayout {
                        spacing: 6
                        DccLabel {
                            Layout.fillWidth: true
                            text: model.display
                            elide: Text.ElideLeft
                            color: parent.palette.windowText
                        }
                        Label {
                            text: "/"
                            color: parent.palette.windowText
                        }
                    }
                    background: Rectangle {
                        property D.Palette backgroundColor: D.Palette {
                            normal: Qt.rgba(0, 0, 0, 0)
                            normalDark: Qt.rgba(0, 0, 0, 0)
                            hovered: Qt.rgba(0, 0, 0, 0.1)
                            hoveredDark: Qt.rgba(1, 1, 1, 0.1)
                            pressed: Qt.rgba(0, 0, 0, 0.06)
                            pressedDark: Qt.rgba(1, 1, 1, 0.06)
                        }
                        radius: 6
                        color: D.ColorSelector.backgroundColor
                        border {
                            color: parent.palette.highlight
                            width: parent.activeFocus ? DS.Style.control.focusBorderWidth : 0
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
                        id: ma
                        anchors.fill: parent
                        onClicked: {
                            root.clicked(model)
                        }
                    }
                }
            }
            DelegateChoice {
                roleValue: 2
                delegate: Control {
                    bottomPadding: 3
                    topPadding: 3
                    leftPadding: 4
                    rightPadding: 4
                    anchors.verticalCenter: parent.verticalCenter
                    onImplicitWidthChanged: root.updateLayout()
                    contentItem: DccLabel {
                        text: model.display
                        elide: Text.ElideLeft
                        color: parent.palette.highlight
                    }
                }
            }
        }
    }
}
