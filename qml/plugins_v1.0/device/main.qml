// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.3

import Dcc 1.0
import org.deepin.dtk 1.0 as D

DccObject {
    DccObject {
        parentName: "device"
        name: "bluetooth"
        displayName: qsTr("bluetooth")
        icon: "bluetooth"
        weight: 10
        pageType: DccObject.Item
        page:Rectangle{
            width: 240
            height: 100
            anchors.margins: 10
            radius: 8

            // D.DciIcon {
            //     id:img
            //     height:50
            //     width:50
            //     anchors.left: parent.left
            //     name: dccObj.icon
            //     sourceSize:Qt.size(32,32)
            // }
            Text {
                // anchors.leftMargin: 20
                id: display
                // width:200
                anchors.bottom: parent.verticalCenter
                // anchors.left: img.right
                text: dccObj.displayName
                // Layout.fillWidth: true
            }
            // Text{
            //     anchors.left:  img.right
            //     anchors.top: display.bottom
            //     // width:200
            //     opacity: 0.5
            //     text:dccObj.name
            // }
            ComboBox {
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter

                model: [qsTr("Top"), qsTr("Bottom"), qsTr("Left"), qsTr("Right")]
            }

            // MouseArea {
            //     anchors.fill: parent
            //     onClicked: {
            //         dccObj.trigger();
            //         console.log(dccObj.name)
            //     }
            // }
        }
    }
    DccObject {
        parentName: "device"
        name: "printer"
        displayName: qsTr("printer")
        icon: "printer"
        weight: 20
    }
    DccObject {
        parentName: "device"
        name: "mouse"
        displayName: qsTr("mouse")
        icon: "mouse"
        weight: 30
        pageType: DccObject.Editor
        page: Text{
            text: "aaaaaaaa"
        }

    }
    DccObject {
        parentName: "device"
        name: "keyboard"
        displayName: qsTr("keyboard")
        description: qsTr("key")
        icon: "keyboard"
        weight: 40
        pageType: DccObject.Editor
        page: Text{
            text: "aaaaaaaa"
        }
    }
    DccObject {
        parentName: "device"
        name: "wacom"
        displayName: qsTr("wacom")
        icon: "wacom"
        weight: 50
    }
}
