import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0
import Deepin.Widgets 1.0
import DBus.Com.Deepin.Daemon.Display 1.0

Item {
    id: displayModule
    anchors.fill: parent

    property var dconstants: DConstants {}
    property var displayId: Display {}
    property var allMonitorsObjects: {
        displayId.ResetChanged()
        var myObjects = new Array()
        var monitors = displayId.monitors
        for(var i=0; i<monitors.length; i++){
            var monitorObj = monitorComponent.createObject(displayModule, { path: monitors[i] })
            myObjects.push(monitorObj)
        }
        return myObjects
    }

    Component {
        id: monitorComponent
        Monitor {}
    }

    Column {
        anchors.top: parent.top
        width: parent.width

        DssTitle {
            text: dsTr("Display")
        }

        DSeparatorHorizontal {}

        MonitorDragArea {}

        DSeparatorHorizontal {}

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
                    text: dsTr("Cancel")
                    onClicked: {
                        displayId.ResetChanged()
                    }
                }
            }
        }
    }
}
