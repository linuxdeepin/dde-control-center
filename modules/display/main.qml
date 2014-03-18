import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0
import Deepin.Widgets 1.0
import DBus.Com.Deepin.Daemon.Display 1.0

Column {
    id: displayModule
    anchors.fill: parent

    property var messageBox: MessageBox{}
    property var dconstants: DConstants {}
    property var displayId: Display {}
    property var allMonitorsObjects: getAllMonitorsObj(displayId.monitors)

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
                    visible: allMonitorsObjects.length > 1
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
