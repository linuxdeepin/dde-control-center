/****************************************************************************
**
**  Copyright (C) 2011~2014 Deepin, Inc.
**                2011~2014 WanQing Yang
**
**  Author:     WanQing Yang <match.yangwanqing@gmail.com>
**  Maintainer: WanQing Yang <match.yangwanqing@gmail.com>
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

import QtQuick 2.0
import QtQuick.Controls 1.1
import QtQuick.Controls.Styles 1.1
import QtQuick.Layouts 1.1
import Deepin.Widgets 1.0

Item {
    id: rotationRec

    visible: pOutputObj.opened
    width: parent.width
    height: visible ? rotationView.height + monitorName.height + 20 : 0

    property var pOutputObj
    property string pOutputObjName: ""
    property bool shouldShowMonitorName: true
    property int itemIndex: 0

    //if the name changed,it must be another monitor Object,data should be clear and reload
    onPOutputObjNameChanged: {
        rotationView.loadRotationModel()
    }


    function getResolutionFromMode(mode){
        return mode[1] + "x" + mode[2]
    }

    Rectangle {
        width: parent.width - 30
        height: visible ? 2 : 0
        color: DConstants.bgColor
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        visible: itemIndex != 0 && shouldShowMonitorName
    }

    DssH2 {
        id:monitorName
        visible: shouldShowMonitorName
        height: visible ? 20 : 0
        anchors.top: parent.top
        anchors.topMargin: 10
        anchors.left: parent.left
        anchors.leftMargin: 20
        color: DConstants.fgDarkColor
        text: dsTr("Monitor") + " " + pOutputObjName
    }

    GridView{
        id: rotationView
        width: parent.width
        height: Math.ceil(count/2) * 30
        anchors.top: monitorName.bottom
        anchors.topMargin: 5
        anchors.horizontalCenter: parent.horizontalCenter

        cellWidth: width/2
        cellHeight: 30
        clip: true
        property int currentValue: pOutputObj.rotation

        //pOutputObj.ListModes() maybe got nothing,try to reload data
        //workaround here
        Timer {
            id:reloadDataTimer
            interval: 1000
            onTriggered: {
                rotationView.loadRotationModel()
            }
        }

        function loadRotationModel(){
            var rotations = pOutputObj.ListRotations()
            if (pOutputObj.opened && rotations.length == 0){
                print (pOutputObj.name,"'s rotations is empty!reload data now...")
                reloadDataTimer.start()//no data refresh
            }

            rotationView.model.clear()
            for(var i=0; i<rotations.length; i++){
                var rotation = rotations[i]
                rotationView.model.append({
                                          "item_label": rotationNames[rotation],
                                          "item_value": rotations[i]
                                      })
            }
        }

        model: ListModel {}

        delegate: RotationItem {
            currentValue: rotationView.currentValue
            onSelectAction: {
                pOutputObj.SetRotation(itemValue)
            }
        }
    }
}
