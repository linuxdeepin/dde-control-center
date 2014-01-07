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
        id: output
        Output {}
    }

    function getOutputObject(path){
        var obj = output.createObject(parent, { path: path })
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

        Repeater {
            model: outputs.length
            delegate: MonitorProperties {
                outputObj: getOutputObject(outputs[index])
            }
        }
    }
}
