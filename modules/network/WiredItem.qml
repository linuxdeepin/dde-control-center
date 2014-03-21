import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import Deepin.Widgets 1.0

Column{
    id: wiredDevicesItem
    width: parent.width
    height: childrenRect.height

    property int wiredDevicesSignal: nm.wiredDevices[index][1]

    //DBaseLine {
        //id:content
        //color: dconstants.contentBgColor

        //leftMargin: 32
        //leftLoader.sourceComponent: DTextAction {
            //text: dsTr("Wired Conections") + " " + index
        //}

        //rightLoader.sourceComponent: DSwitchButton {
            //checked: wiredDevicesSignal == 100
            //onClicked: {
                //if (checked) {
                    //nm.ActiveWiredDevice(checked, nm.wiredDevices[index][0])
                //} else {
                    //nm.DisconnectDevice(nm.wiredDevices[index][0])
                //}
            //}

            //Connections{
                //target: wiredDevicesItem
                //onWiredDevicesSignalChanged:{
                    //parent.checked = (wiredDevicesSignal == 100)
                //}
            //}
        //}
    //}

    DBaseLine {
        id: wiredLine

        property bool hovered: false
        property bool selected: false
        color: dconstants.contentBgColor

        MouseArea{
            z:-1
            anchors.fill:parent
            hoverEnabled: true

            onEntered: {
                parent.hovered = true
            }

            onExited: {
                parent.hovered = false
            }

            onClicked: {
                nm.ActiveWiredDevice(nm.wiredDevices[index][0])
            }
        }

        leftLoader.sourceComponent: Item {
            height: childrenRect.height
            anchors.verticalCenter: parent.verticalCenter

            DImageButton {
                anchors.left: parent.left
                normal_image: "img/check_1.png"
                hover_image: "img/check_2.png"
                visible: wiredDevicesSignal == 100
                onClicked: {
                    nm.DisconnectDevice(nm.wiredDevices[index][0])
                }

                Connections{
                    target: wiredDevicesItem
                    onWiredDevicesSignalChanged:{
                        parent.visible = (wiredDevicesSignal == 100)
                    }
                }
            }

            DLabel {
                anchors.left: parent.left
                anchors.leftMargin: 18
                verticalAlignment: Text.AlignVCenter
                text: dsTr("Wired Conections") + " " + index
                color: {
                    if(wiredLine.selected){
                        return dconstants.activeColor
                    }
                    else if(wiredLine.hovered){
                        return dconstants.hoverColor
                    }
                    else{
                        return dconstants.fgColor
                    }
                }
            }
        }

        rightLoader.sourceComponent: DArrowButton {
            onClicked: {
                stackView.push({
                    "item": Qt.resolvedUrl("WirelessProperties.qml"),
                    //"properties": { "accessPoint": accessPoints[index]},
                    "destroyOnPop": true
                })
            }
        }
    }
    
    DSeparatorHorizontal{}
}
