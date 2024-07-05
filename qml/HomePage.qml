// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.3

import org.deepin.dtk 1.0

import Dcc 1.0
/*
Rectangle{
    width: 240
    height:64
    visible: true
    anchors.margins:  10
    radius:8
    color: "red"
}*/

Page{
    id: root
    SearchEdit {
        id: searchEdit
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.margins: 10

        // Layout.alignment: Qt.AlignHCenter
        implicitWidth: (parent.width / 2) > 280 ? 280 : (parent.width / 2)
    }

GridView {
        id: grid
        // x: 10
        // y: 10
        // visible: true
        anchors.top: searchEdit.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: 10
        // anchors.fill: parent
        clip: true
        cellWidth: 250
        cellHeight: 69
        activeFocusOnTab:true

        Keys.enabled: true
        Keys.onPressed: {
            switch (event.key) {
                case Qt.Key_Enter:
                case Qt.Key_Return:
                    var obj = dccModel.getObject(currentIndex);
                    if (obj) {
                        obj.trigger();
                    }
                    break
                default:
                    return
            }
            event.accepted = true
        }

        highlight: Item {
            z: 2
            FocusBoxBorder {
                anchors {
                    fill: parent
                    margins: 5
                }
                radius: isWindowedMode ? 8 : 18
                color: parent.palette.highlight
                visible: grid.activeFocus
            }
            Rectangle {
                anchors {
                    fill: parent
                    margins: 5
                }
                radius: 18
                color: Qt.rgba(0, 0, 0, 0.2)
                visible: grid.activeFocus
            }
        }
        focus: true
        model: DccModel {
            id: dccModel
            root: dccObj
        }
        delegate:
        Rectangle{
            width: 240
            height: 64
            anchors.margins: 10
            radius: 8

            DciIcon {
                id: img
                height: 50
                width: 50
                anchors.left: parent.left
                name: model.item.icon
                sourceSize: Qt.size(32, 32)
            }
            Text {
                // anchors.leftMargin: 20
                id: display
                // width:200
                anchors.bottom: parent.verticalCenter
                anchors.left: img.right
                text: model.display
                // Layout.fillWidth: true
            }
            Text{
                anchors.left: img.right
                anchors.top: display.bottom
                // width:200
                opacity: 0.5
                text: model.item.name
            }
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    model.item.trigger();
                    console.log(model.item.name)
                }
            }
        }
    }
}

