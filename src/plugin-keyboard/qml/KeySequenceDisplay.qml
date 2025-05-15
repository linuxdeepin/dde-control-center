// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import org.deepin.dtk 1.0 as D
import org.deepin.dtk.style 1.0 as DS
import org.deepin.dtk.private 1.0 as P

Control {
    id: control
    property string text
    property string placeholderText
    property list<string> keys
    property D.Palette backgroundColor: DS.Style.keySequenceEdit.background
    property D.Palette placeholderTextColor: DS.Style.keySequenceEdit.placeholderText
    property bool showEditButtons: false
    property bool showWarnning: false
    signal requestKeys
    signal requestEditKeys
    signal requestDeleteKeys

    background: Rectangle {
        implicitWidth: DS.Style.keySequenceEdit.width
        implicitHeight: DS.Style.keySequenceEdit.height
        radius: DS.Style.control.radius
        color: control.D.ColorSelector.backgroundColor
    }

    contentItem: RowLayout {
        Label {
            Layout.leftMargin: DS.Style.keySequenceEdit.margin
            font: D.DTK.fontManager.t6
            textFormat: Text.PlainText
            text: control.text
            horizontalAlignment: Qt.AlignLeft
            verticalAlignment: Qt.AlignVCenter
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
        }

        Component {
            id: inputComponent
            Label {
                text: control.placeholderText
                color: control.D.ColorSelector.placeholderTextColor
                font: D.DTK.fontManager.t7
                horizontalAlignment: Qt.AlignRight
                verticalAlignment: Qt.AlignVCenter
            }
        }

        Component {
            id: keyComponent
            RowLayout {
                spacing: DS.Style.keySequenceEdit.margin
                D.IconButton {
                    id: warnningBtn
                    flat: true
                    background: null
                    visible: showWarnning
                    icon.name: "icon_info"
                    icon.width: 24
                    icon.height: 24
                }

                Repeater {
                    model: control.keys
                    P.KeySequenceLabel {
                        visible: !control.showEditButtons
                        Layout.alignment: Qt.AlignRight
                        text: modelData
                    }
                }

                D.IconButton {
                    id: editButton
                    focusPolicy: Qt.NoFocus
                    visible: control.showEditButtons
                    icon.name: "edit"
                    icon.width: 16
                    icon.height: 16
                    implicitWidth: 16
                    implicitHeight: 16
                    background: null
                    onClicked: {
                        control.requestEditKeys()
                    }
                }
                D.IconButton {
                    id: removeButton
                    focusPolicy: Qt.NoFocus
                    visible: control.showEditButtons
                    icon.name: "user-trash-symbolic"
                    icon.width: 24
                    icon.height: 24
                    implicitWidth: 36
                    implicitHeight: 36
                    background: null
                    onClicked: {
                        control.requestDeleteKeys()
                    }
                }
            }
        }

        Loader {
            Layout.rightMargin: DS.Style.keySequenceEdit.margin
            Layout.alignment: Qt.AlignVCenter
            sourceComponent: (control.keys.length !== 0) ? keyComponent : inputComponent

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    control.requestKeys()
                }
            }
        }
    }
}
