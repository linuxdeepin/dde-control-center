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
    property bool isGroup: false
    property alias control: root.parent
    property int position: isGroup ? updatePosition() : 0

    Loader {
        anchors.fill: parent
        active: !checked && control.backgroundVisible
        sourceComponent: D.RoundRectangle {
            color: palette.base
            radius: DS.Style.control.radius
            corners: control.corners
        }
    }
    Loader {
        id: rectBg
        anchors {
            left: parent.left
            right: parent.right
        }
        active: position != 0 && position != 4
        sourceComponent: Rectangle {
            color: palette.base
        }
        function updateItemBg() {
            switch (position) {
            case 1:
                // Beginning
                anchors.top = parent.verticalCenter
                anchors.bottom = parent.bottom
                break
            case 2:
                // Middle
                anchors.top = parent.top
                anchors.bottom = parent.bottom
                break
            case 3:
                // End
                anchors.top = parent.top
                anchors.bottom = parent.verticalCenter
                break
            default:
                break
            }
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

    function updatePosition() {
        let count = 0
        if (model.count) {
            count = model.count
        } else if (control.ListView.view) {
            count = control.ListView.view.count
        }

        if (index == 0) {
            position = count === 1 ? 4 : 1
        } else if (index === count - 1) {
            position = 3
        } else {
            position = 2
        }
        return position
    }
    onPositionChanged: rectBg.updateItemBg()
}
