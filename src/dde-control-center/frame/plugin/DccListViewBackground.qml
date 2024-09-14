// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Qt.labs.platform 1.1

import org.deepin.dtk 1.0 as D
import org.deepin.dtk.style 1.0 as DS

Item {
    id: root
    implicitWidth: DS.Style.itemDelegate.width
    implicitHeight: DS.Style.itemDelegate.height
    property bool separatorVisible: false
    property bool highlightEnable: true
    property alias control: root.parent
    property real radius: DS.Style.control.radius

    Loader {
        anchors.fill: parent
        active: (!highlightEnable || !checked) && control.backgroundVisible
        sourceComponent: D.RoundRectangle {
            color: palette.base
            radius: root.radius
            corners: control.corners
        }
    }

    Loader {
        active: separatorVisible && index !== 0
        height: 2
        anchors {
            bottom: parent.top
            bottomMargin: control.ListView.view ? (control.ListView.view.spacing - 2) / 2 : -1
            // bottomMargin: (control.ListView.view.spacing - 1) / 2
            left: parent.left
            leftMargin: 10
            right: parent.right
            rightMargin: 10
        }
        sourceComponent: Rectangle {
            color: palette.window
        }
    }
    Loader {
        anchors.fill: parent
        anchors.topMargin: 1
        anchors.bottomMargin: 1
        active: highlightEnable && checked && !control.cascadeSelected
        sourceComponent: D.HighlightPanel {}
    }
}
