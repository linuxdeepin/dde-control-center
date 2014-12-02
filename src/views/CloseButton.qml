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

Item {
    id: closeButton
    anchors.top: parent.top
    anchors.right: parent.right
    width: closeImage.width
    height: closeImage.height
    
    signal clicked
    
    Rectangle {
        id: closeBackground
        anchors.fill: parent
        anchors.topMargin: 3
        anchors.rightMargin: 3
        anchors.bottomMargin: 1
        anchors.leftMargin: 1
        color: Qt.rgba(0, 0, 0, 0)
    }
    
    Image {
        id: closeImage
        source: "images/window_close.png"
    }
    
    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        
        onEntered: {
            closeBackground.color = Qt.rgba(1, 1, 1, 0.3)
        }
        
        onExited: {
            closeBackground.color = Qt.rgba(1, 1, 1, 0)
        }
        
        onClicked: {
            closeButton.clicked()
        }
    }
}
