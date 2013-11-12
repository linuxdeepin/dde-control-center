import QtQuick 2.1
import QtQuick.Controls 1.0

Item {
    id: systemInfoTopItem
    Component.onCompleted: {
        var info = module_system_info.get_systeminfo()
        var info = JSON.parse(info)
        for (var key in info) {
            myInfo.append({"info": key + " : " + info[key]})
        } 
    }

    Rectangle {
        color: "#252627"
        anchors.fill: parent

        ListView {
            delegate: InfoLine{}
            model: ListModel {id:myInfo}
            anchors.fill: parent
            anchors.topMargin: 10
        }
    }
}
