import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0
import Deepin.Widgets 1.0
import DBus.Com.Deepin.Daemon.Display 1.0

Item {
    id: displayModule
    anchors.fill: parent

    property var dconstants: DConstants {}
    property var displayId: Display {
        onPrimaryChanged: {
            print("==> [debug] primary:", displayId.primary)
        }
    }
    property var allMonitorsObjects: getAllMonitorsObj(displayId.monitors)

    function getAllMonitorsObj(monitors){
            var monitorsObjects = new Array()
            for(var i=0; i<monitors.length; i++){
                var monitorObj = monitorComponent.createObject(displayModule, { path: monitors[i] })
                monitorsObjects.push(monitorObj)
            }
            return monitorsObjects
    }
    
    Component.onCompleted: {
        if(!windowView.getDisplayConfigExists()){
            displayId.saveChanged()
        }
    }

    Component {
        id: monitorComponent
        Monitor {}
    }

    Column {
        id: topColumn
        anchors.top: parent.top
        width: parent.width

        DssTitle {
            text: dsTr("Display")
        }

        DSeparatorHorizontal {}

        MonitorDragArea {
            id: monitorDragArea
        }

        DSeparatorHorizontal {}
    }

    Column{
        id: propertiesColumn
        anchors.top: topColumn.bottom
        width: parent.width
        visible: !monitorDragArea.editable

        DBaseLine {
            id: monitorChoose
            height: 38

            property var currentSelectedMonitor: rightLoader.item.currentItem.delegateId

            leftLoader.sourceComponent: DssH2 {
                text: dsTr("Monitor")
            }

            rightLoader.sourceComponent: DRadioButton {
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

        DBaseLine {
            visible: displayId.hasChanged
            rightMargin: 10
            rightLoader.sourceComponent: Row {
                spacing: 6
                DTextButton {
                    text: dsTr("Apply")
                    onClicked: {
                        displayId.Apply()
                    }
                }
                DTextButton{
                    text: dsTr("Reset")
                    onClicked: {
                        displayId.ResetChanged()
                    }
                }
            }
        }
    }
}
