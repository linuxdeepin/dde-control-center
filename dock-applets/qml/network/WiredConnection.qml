import QtQuick 2.0
import Deepin.Widgets 1.0

ListView {
    width: parent.width
    height: childrenRect.height
    model: wiredDeviceList

    delegate: Column {
        width: parent.width

        DSwitchButtonHeader{
            color: "transparent"
            text: {
                if (wiredDeviceList.length > 1){
                    return dsTr("Wired Network %1").arg(index + 1)
                }
                else{
                    return dsTr("Wired Network")
                }
            }

            onClicked: {
            }
        }

        DSeparatorHorizontal {
            visible: index != wiredDeviceList.length - 1
            width: parent.width * 0.9
            anchors.horizontalCenter: parent.horizontalCenter
        }
    }
}
