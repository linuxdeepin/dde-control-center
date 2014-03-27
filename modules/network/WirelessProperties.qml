import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import Deepin.Widgets 1.0
import QtQuick.Controls 1.1

Column{
    id: wirelessProperties
    width: parent.width

    property var accessPoint

    property int activeExpandIndex: 0
    property int valueWidth: 180

    DBaseLine{
        height: 38
        leftLoader.sourceComponent: DssH2{
            text: "Access Point :  " + accessPoint[0]
        }
    }

    DSeparatorHorizontal {}

    DBaseExpand{
        id: ipv4Settings
        property int myIndex: 0

        expanded: activeExpandIndex == myIndex
        onExpandedChanged: {
            if(header.item){
                header.item.active = expanded
            }
        }

        header.sourceComponent: DDownArrowHeader{
            text: dsTr("IPv4 Settings")
            onClicked: {
                if(activeExpandIndex == root.myIndex){
                    activeExpandIndex = -1
                }
                else{
                    activeExpandIndex = root.myIndex
                }
            }
            Component.onCompleted: {
                active = (activeExpandIndex == root.myIndex)
            }
        }

        content.sourceComponent: Column {
            DBaseLine {
                leftLoader.sourceComponent: DssH2{
                    text: dsTr("Method")
                }

                rightLoader.sourceComponent: DComboBox{
                }
            }
        }
    }

}
