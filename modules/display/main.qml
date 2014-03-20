import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0
import Deepin.Widgets 1.0
import DBus.Com.Deepin.Daemon.Display 1.0

Column {
    id: displayModule
    anchors.fill: parent

    Connections{
        target: rootWindow

        onModuleStartChange: {
            displayId.ResetChanged()
        }

        onDisplayWidthChanged: {
            if(rootWindow.displayWidth == 0){
                monitorDragArea.editable = false
                displayId.ResetChanged()
            }
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
    property var monitorNameDict: new Object()

    function getDisplayMonitorName(outputObj){
        if(outputObj.isComposited){
            var names = Object.keys(outputObj.brightness)
            var numberNames = new Array()
            for(var i in names){
                numberNames.push(String(monitorNameDict[names[i]]))
            }
            return windowView.joinString(numberNames, "=")
        }
        else{
            return monitorNameDict[outputObj.name]
        }
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
            if(monitorObj.isComposited){
                var names = Object.keys(monitorObj.brightness)
                for(var i in names){
                    var name = names[i]
                    if(!monitorNameDict[name]){
                        monitorNameDict[name] = Object.keys(monitorNameDict).length + 1
                    }
                }
            }
            else{
                var name = monitorObj.name
                if(!monitorNameDict[name]){
                    monitorNameDict[name] = Object.keys(monitorNameDict).length + 1
                }
            }
        }
        return monitorsObjects
    }
    
    Component.onCompleted: {
        displayId.SaveChanged()
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
                    text: dsTr("Display")
                    color: "white"
                    font.bold: true
                }

                DssH3 {
                    visible: openedMonitors.length > 1
                    anchors.bottom: moduleName.bottom
                    text: "(" + dsTr("Main Display: ") + displayId.primary + ")"
                    color: "white"
                }
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
                            "buttonLabel": " " + getDisplayMonitorName(outputObj) + " "
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
                    displayId.ResetChanged()
                    monitorDragArea.editable = false
                }
            }
        }
    }
}
