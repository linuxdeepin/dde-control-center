// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
// import org.deepin.dtk 1.0
import Qt.labs.platform 1.1

import org.deepin.dtk 1.0 as D
import org.deepin.dtk.style 1.0 as DS
Item {
    id:root
    implicitWidth: DS.Style.itemDelegate.width
    implicitHeight: DS.Style.itemDelegate.height
    property bool separatorVisible: true
    property alias control: root.parent
    Loader {
        anchors.fill: parent
        active: checked && !control.cascadeSelected
        sourceComponent: D.HighlightPanel {}
    }

    Loader {
        anchors.fill: parent
        active: checked && control.cascadeSelected
        sourceComponent: D.RoundRectangle {
            color: "white" //DS.Style.itemDelegate.cascadeColor
            radius: DS.Style.control.radius
            corners: control.corners
        }
    }

    Loader {
        anchors.fill: parent
        active: !checked && control.backgroundVisible
        sourceComponent: D.RoundRectangle {
            color: "white" //DS.Style.itemDelegate.normalColor
            radius: DS.Style.control.radius
            corners: control.corners
        }
    }
    Loader {
        active: separatorVisible && index !== 0
        height: 1
        anchors {
            bottom:  parent.top
            bottomMargin: control.ListView.view ? (control.ListView.view.spacing - 1) / 2 : 0.5
            // bottomMargin: (control.ListView.view.spacing - 1) / 2
            left: parent.left
            leftMargin: 10
            right: parent.right
            rightMargin: 10
        }
        sourceComponent: Rectangle {
            color: "gray"
        }
        // Component.onCompleted: {
        //     console.log("====control.ListView=====",control.ListView)
        //     console.log("====control.ListView.view=====",control.ListView.view)
        //     console.log("=====control.ListView.view.spacing====",(control.ListView.view.spacing - 0) / 2)
        //     console.log("=====control.ListView.view.spacing====",(control.ListView.view.spacing - 1) / 2)
        // }
    }
}
