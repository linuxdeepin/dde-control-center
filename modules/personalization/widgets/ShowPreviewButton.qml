/****************************************************************************
**
**  Copyright (C) 2011~2014 Deepin, Inc.
**                2011~2014 Kaisheng Ye
**
**  Author:     Kaisheng Ye <kaisheng.ye@gmail.com>
**  Maintainer: Kaisheng Ye <kaisheng.ye@gmail.com>
**
**  This program is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

import QtQuick 2.1

Image {
    id: actionButton

    source: "images/preview.png"

    opacity: 1.0
    property alias mouseArea: mouseArea

    signal clicked
    
    states: State {
        name: "hovered"
        PropertyChanges { target: actionButton; opacity: 1.0 }
    }
    
    transitions: Transition {
        NumberAnimation { properties: "opacity"; duration: 350 }
    }
    
    MouseArea {
        id: mouseArea
        anchors.fill: actionButton
        hoverEnabled: true
        
        onEntered: {
            actionButton.state = "hovered"
            mouseArea.cursorShape = Qt.PointingHandCursor
            toolTip.showTip(dsTr("Preview Theme"))
        }
        
        onExited: {
            actionButton.state = ""
            mouseArea.cursorShape = Qt.ArrowCursor
            toolTip.hideTip()
        }
        
        onReleased: { 
            actionButton.state = mouseArea.containsMouse ? "hovered" : ""
        }
        onClicked: {
            parent.clicked()
        }
    }
}

