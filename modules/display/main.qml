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
    property var outputs: displayId.outputs

    Component {
        id: outputComponent
        Output {}
    }

    Component {
        id: listModelComponent
        ListModel {}
    }

    function getOutputObject(path){
        var obj = outputComponent.createObject(parent, { path: path })
        return obj
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
            height: 40
            leftLoader.sourceComponent: DssH2 {
                text: dsTr("Primary Monitor")
            }
            rightLoader.sourceComponent: Item {
                width: childrenRect.width
                height: childrenRect.height
                property alias currentOutput: chooser.currentIndex

                DRadioButton {
                    id: chooser
                    currentIndex: 0
                    buttonModel: {
                        var myModel = new Array()
                        for(var i=0; i<outputs.length; i++){
                            var outputObj = getOutputObject(outputs[i])
                            myModel.push({
                                "buttonId": outputObj,
                                "buttonLabel": outputObj.name
                            })
                        }
                        return myModel
                    }

                    onItemSelected: {
                    }
                    visible: true
                }

            }
        }

        DSeparatorHorizontal{}

        Repeater{
            model: outputs.length
            delegate: MonitorProperties {
                outputObj: getOutputObject(outputs[index])
            }
        }

        DBaseLine {
            rightMargin: 10
            rightLoader.sourceComponent: Component {
                DTextButton {
                    text: dsTr("Apply")
                }
            }
        }
    }
}
