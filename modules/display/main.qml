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

import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0
import Deepin.Widgets 1.0
import DBus.Com.Deepin.Daemon.Display 1.0
import "../shared"

Column {
    id: displayModule
    anchors.fill: parent

    Connections{
        target: rootWindow

        onModuleStartChange: {
            displayId.ResetChanges()
        }

        onPanelHided: {
            monitorDragArea.editable = false
            displayId.ResetChanges()
        }
    }

    property var messageBox: MessageBox{}
    property var dconstants: DConstants {}
    property var displayId: Display {}

    property var allMonitorsObjects: getAllMonitorsObj(displayId.monitors)
    property var openedMonitors: {
        var openedM = new Array()
        for(var i in allMonitorsObjects){
            if(allMonitorsObjects[i].opened){
                openedM.push(allMonitorsObjects[i])
            }
        }
        return openedM
    }

    function displayChangesApply(){
        displayId.Apply()
        messageBox.showDialog()
    }

    function getAllMonitorsObj(monitors){
        var monitorsObjects = new Array()
        for(var i=0; i<monitors.length; i++){
            var monitorObj = monitorComponent.createObject(displayModule, { path: monitors[i] })
            monitorsObjects.push(monitorObj)
        }
        return monitorsObjects
    }

    function getMonitorNames(outputObj){
        return Object.keys(outputObj.brightness)
    }
    
    Component.onCompleted: {
        displayId.SaveChanges()
    }

    Component {
        id: monitorComponent
        Monitor {}
    }

    Column {
        id: topColumn
        width: parent.width
        height: childrenRect.height

        DBaseLine{
            height: 48
            leftLoader.sourceComponent: Row{
                spacing: 4
                height: moduleName.height
                anchors.verticalCenter: parent.verticalCenter

                DssH1 {
                    id: moduleName
                    text: modulesId.moduleLocaleNames["display"]
                    color: "white"
                    font.bold: true

                    MouseArea{
                        anchors.fill: parent
                        onClicked: messageBox.showDialog()
                    }
                }

                DssH3 {
                    visible: openedMonitors.length > 1
                    anchors.bottom: moduleName.bottom
                    text: "(" + dsTr("Main Display: ") + displayId.primary + ")"
                    color: "white"
                }
            }
            rightLoader.sourceComponent: ResetButton {
                onClicked: displayId.Reset()
            }
        }

        DSeparatorHorizontal {}

        MonitorDragArea {
            id: monitorDragArea
        }

        DSeparatorHorizontal {}
    }

    Column{
        id: propertiesColumn
        width: parent.width
        height: childrenRect.height
        visible: !monitorDragArea.editable

        DBaseLine {
            id: monitorChoose
            height: 38

            property var currentSelectedMonitor: rightLoader.item.currentItem.delegateId
            
            leftLoader.sourceComponent: DssH2 {
                text: dsTr("Monitor")
            }

            rightLoader.sourceComponent: DRadioButton {

                function getIndexFromId(buttonId){
                    for(var i=0; i<buttonModel.length; i++){
                        if(buttonId == buttonModel[i].buttonId){
                            return i
                        }
                    }
                    return -1
                }

                function selectItemFromId(buttonId){
                    var index = getIndexFromId(buttonId)
                    if(index != -1){
                        selectItem(index)
                        return true
                    }
                    else{
                        return false
                    }
                }

                buttonModel: {
                    var myModel = new Array()
                    for(var i=0; i<allMonitorsObjects.length; i++){
                        var outputObj = allMonitorsObjects[i]
                        myModel.push({
                            "buttonId": outputObj,
                            "buttonLabel": outputObj.name
                        })
                    }
                    return myModel
                }
            }
        }

        DSeparatorHorizontal{}

        MonitorProperties {
            outputObj: monitorChoose.currentSelectedMonitor
            monitorsNumber: allMonitorsObjects.length
        }
    }

    DBaseLine {
        rightMargin: 10
        rightLoader.sourceComponent: Row {
            spacing: 6
            DTextButton {
                text: dsTr("Apply")
                visible: { 
                    if (monitorDragArea.editable || displayId.hasChanged){
                        return true
                    }
                    else{
                        return false
                    }
                } 
                onClicked: {
                    if(monitorDragArea.editable){
                        monitorDragArea.applyPostion()
                        monitorDragArea.editable = false
                    }
                    displayChangesApply()
                }
            }

            DTextButton {
                text: { 
                    if(displayId.hasChanged){
                        return dsTr("Reset")
                    }
                    else{
                        return dsTr("Cancel")
                    }
                }

                visible: { 
                    if (monitorDragArea.editable || displayId.hasChanged){
                        return true
                    }
                    else{
                        return false
                    }
                } 

                onClicked: {
                    if(monitorDragArea.editable){
                        monitorDragArea.editable = false
                    }
                    displayId.ResetChanges()
                    monitorDragArea.editable = false
                }
            }
        }
    }
}
