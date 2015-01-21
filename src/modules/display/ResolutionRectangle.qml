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
    id: resolutionRec

    visible: pOutputObj.opened
    width: parent.width
    height: visible ? modesView.height + monitorName.height + 20 : 0

    property var pOutputObj
    property string pOutputObjName: ""
    property bool shouldShowMonitorName: true
    property int itemIndex: 0

    function getResolutionFromMode(mode){
        return mode[1] + "x" + mode[2]
    }

    //if the name changed,it must be another monitor Object,data should be clear and reload
    onPOutputObjNameChanged: {
        modesView.loadResolutionModel()
    }

    Rectangle {
        width: parent.width - 30
        height: visible ? 2 : 0
        color: dconstants.bgColor
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        visible: itemIndex != 0 && shouldShowMonitorName
    }

    DssH2 {
        id:monitorName
        visible: shouldShowMonitorName && modesView.count > 0
        height: visible ? 20 : 0
        anchors.top: parent.top
        anchors.topMargin: 10
        anchors.left: parent.left
        anchors.leftMargin: 20
        color: dconstants.fgDarkColor
        text: dsTr("Monitor") + " " + pOutputObjName
    }

    GridView{
        id: modesView
        width: parent.width
        height: Math.ceil(count/3) * 30
        anchors.top: monitorName.bottom
        anchors.topMargin: 5
        anchors.horizontalCenter: parent.horizontalCenter

        cellWidth: width/3
        cellHeight: 30
        clip: true
        property string currentValue: (pOutputObj.width).toString() + "x" + (pOutputObj.height).toString()
        property var valueDict: new Object()

        //pOutputObj.ListModes() maybe got nothing,try to reload data
        //workaround here
        Timer {
            id:reloadDataTimer
            interval: 1000
            onTriggered: {
                modesView.loadResolutionModel()
            }
        }

        function loadResolutionModel(){
            var modes = pOutputObj.ListModes()
            if (pOutputObj.opened && modes.length == 0){
                print (pOutputObj.name,"'s resolution is empty!reload data now...")
                reloadDataTimer.start()
            }

            modesView.model.clear()
            valueDict = new Object()
            for(var i=0; i<modes.length; i++){
                var resolution = getResolutionFromMode(modes[i])
                if(!valueDict[resolution]){
                    valueDict[resolution] = []
                }
                valueDict[resolution].push(modes[i][0])
            }

            for(var key in valueDict){
                modesView.model.append({
                                           "item_label": key,
                                           "item_value": valueDict[key][0]
                                       })
            }
        }

        model: ListModel{}

        delegate: ModeItem {
            modesDict: modesView.valueDict
            currentValue: modesView.currentValue
            onSelectAction: {
                pOutputObj.SetMode(itemValue)
            }
        }
    }

}
