// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.11
import QtQuick.Layouts 1.15
import QtQuick.Templates as T
import org.deepin.dtk 1.0 as D
import org.deepin.dtk.style 1.0 as DS
import org.deepin.dcc 1.0

T.Control {
    id: control
    property string text: "ImageCheckBox"
    property bool checked: true
    property string imageName: ""
    implicitWidth: contentItem.implicitWidth
    implicitHeight: contentItem.implicitHeight
    Keys.onPressed: function(event) {
        if (event.key === Qt.Key_Space || event.key === Qt.Key_Return) {
            checked = !checked
        }
    }
    MouseArea {
        anchors.fill: parent
        onClicked: {
            checked = !checked
        }
    }
    ColumnLayout {
        id: contentItem
        anchors.fill: parent
        
        Item {
            Layout.alignment: Qt.AlignCenter
            implicitWidth: 96
            implicitHeight: 65
            focus: true
            activeFocusOnTab: true

            Rectangle {
                anchors.fill: parent
                visible: parent.activeFocus
                color: "transparent"
                border.width: 2
                border.color: control.palette.highlight
                radius: DS.Style.control.radius
                z: 1
            }

            Image {
                anchors.centerIn: parent
                sourceSize: Qt.size(96, 65)
                mipmap: true
                source: D.DTK.themeType === D.ApplicationHelper.LightType ?
                    "qrc:/icons/deepin/builtin/light/icons/" + imageName +"_84px.png" : "qrc:/icons/deepin/builtin/dark/icons/" + imageName +"_84px.png"
            }
        }
        
        RowLayout {
            Layout.alignment: Qt.AlignCenter
            Item {
                Layout.alignment: Qt.AlignCenter
                implicitWidth: 24
                implicitHeight: 24
                focus: true
                activeFocusOnTab: true

                Rectangle {
                    anchors.fill: parent
                    visible: parent.activeFocus
                    color: "transparent"
                    border.width: 2
                    border.color: control.palette.highlight
                    radius: DS.Style.control.radius
                    z: 1
                }

                DccCheckIcon {
                    anchors.centerIn: parent
                    checked: control.checked
                    activeFocusOnTab: false
                    onClicked: {
                        control.checked = !control.checked
                    }
                }
            }
            Text {
                Layout.alignment: Qt.AlignCenter
                text: control.text
                font: control.font
                elide: Text.ElideRight
                verticalAlignment: Text.AlignVCenter
                color: control.palette.windowText
            }
        }
    }
}
