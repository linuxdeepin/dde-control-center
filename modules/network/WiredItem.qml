import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import Deepin.Widgets 1.0

Column{
    id: wiredDevicesItem
    width: parent.width
    height: childrenRect.height

    property int wiredDevicesSignal: nm.wiredDevices[index][1]

    DBaseLine {
        id:content
        color: dconstants.contentBgColor

        leftLoader.sourceComponent: DTextAction {
            text: dsTr("Wired Conections") + " " + index
        }

        rightLoader.sourceComponent: DSwitchButton {
            checked: wiredDevicesSignal == 100
            onClicked: {
                if (checked) {
                    nm.ActiveWiredDevice(checked, nm.wiredDevices[index][0])
                } else {
                    nm.DisconnectDevice(nm.wiredDevices[index][0])
                }
            }

            Connections{
                target: wiredDevicesItem
                onWiredDevicesSignalChanged:{
                    parent.checked = (wiredDevicesSignal == 100)
                }
            }
        }
    }
    
    DSeparatorHorizontal{}
}
