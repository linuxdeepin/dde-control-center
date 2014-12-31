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
    property int allMonitorsObjectsCount: allMonitorsObjects.length
    property var openedMonitors: {
        var openedM = new Array()
        for(var i in allMonitorsObjects){
            if(allMonitorsObjects[i].opened){
                openedM.push(allMonitorsObjects[i])
            }
        }
        return openedM
    }
    property bool editting:{
        if (monitorDragArea.editable || displayId.hasChanged){
            return true
        }
        else{
            return false
        }
    }
    property var realMonitorsList: Object.keys(displayId.brightness)
    property int realMonitorsCount: realMonitorsList.length
    property bool hasCompositedMonitors:{
        for (var i = 0; i < allMonitorsObjectsCount; i ++){
            if (allMonitorsObjects[i].opened && allMonitorsObjects[i].isComposited){
                return true
            }
        }
        return false
    }
    property bool doSplited: false

    property bool onCustomizeMode: false

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
                    font.weight: Font.DemiBold
                }
            }
            rightLoader.sourceComponent: ResetButton {
                onClicked: displayId.Reset()
            }
        }

        DSeparatorHorizontal {}

    }

    ListView {
        height: parent.height - topColumn.height
        width: parent.width
        model: propertyModel
        clip: true
    }

    VisualItemModel
    {
        id: propertyModel


        MonitorDragArea {
            id: monitorDragArea
        }

        DSeparatorHorizontal {}

        MonitorProperties {
            id:monitorsDetailsProperty
            height: visible ? childrenHeight : 0
            property int childrenHeight: childrenRect.height
            visible: (realMonitorsCount == 1 || onCustomizeMode) && !monitorDragArea.editable
        }

        MonitorsSimpleSetting {
            id:simplepropertySetting
            height: visible ? childrenRect.height : 0
            visible: realMonitorsCount > 1 && !monitorsDetailsProperty.visible  && !monitorDragArea.editable
        }


        DBaseLine {
            height: visible ? 30 : 0
            visible: onCustomizeMode || (realMonitorsCount == 1 && editting)
            rightMargin: 10
            rightLoader.sourceComponent: Row {
                spacing: 6
                DTextButton {
                    text: dsTr("Cancel")

                    onClicked: {
                        onCustomizeMode = false
                        if(monitorDragArea.editable){
                            monitorDragArea.editable = false
                        }
                        displayId.ResetChanges()
                        monitorDragArea.editable = false
                        doSplited = false
                    }
                }

                DTextButton {
                    id: applyButton
                    text: displayId.hasChanged || monitorDragArea.editable ? dsTr("Apply") :  dsTr("Confirm")
                    onClicked: {
                        if(monitorDragArea.editable){
                            monitorDragArea.applyPostion()
                            monitorDragArea.editable = false
                        }
                        if (displayId.displayMode != 0){
                            print ("==>[info] change mode to customize...")
                            displayId.SaveChanges()
                            displayId.SwitchMode(0,"")
                            onCustomizeMode = false
                        }
                        else if(displayId.hasChanged){
                            displayChangesApply()
                        }
                        else{
                            onCustomizeMode = false
                        }
                        doSplited = false
                    }
                }
            }
        }

    }
}
