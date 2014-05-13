import QtQuick 2.1
import Deepin.Widgets 1.0
import "components_section_autogen"

BaseConnectionProperties {

    ParallelAnimation {
        id: showDelete
        NumberAnimation {
            target: saveBox; 
            property: "x"; 
            to: buttonLine.x + buttonLine.width - saveBox.width - deleteBox.width; 
            duration: 200 
        }
        NumberAnimation {
            target: saveBox; 
            property: "opacity"; 
            to: 0; 
            duration: 200 
        }
    }

    ParallelAnimation {
        id: cancelDelete
        NumberAnimation {
            target: saveBox; 
            property: "x"; 
            to: buttonLine.x + buttonLine.width - saveBox.width; 
            duration: 200 
        }
        NumberAnimation {
            target: saveBox; 
            property: "opacity"; 
            to: 1; 
            duration: 200 
        }
    }

    Column {
        width: parent.width
        visible: isActiveConnection(devicePath, uuid)
        DBaseLine {
            id: deleteSettingLine
            color: dconstants.contentBgColor

            property bool inDeleteSetting: false
            property bool hovered: false
            property bool pressed: false

            leftLoader.sourceComponent: DssH2 {
                text: deleteSettingLine.inDeleteSetting ? dsTr("Are you sure to delete this network?") : dsTr("Delete this network")
                color: {
                    if(deleteSettingLine.inDeleteSetting){
                        return dconstants.tuhaoColor
                    }
                    else if(deleteSettingLine.pressed){
                        return dconstants.activeColor
                    }
                    else if (deleteSettingLine.hovered){
                        return dconstants.hoverColor
                    }
                    else{
                        return dconstants.fgColor
                    }
                }
            }

            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
                onEntered: parent.hovered = true
                onExited: parent.hovered = false
                onPressed: parent.pressed = true
                onReleased: { parent.pressed = false; parent.hovered = containsMouse }
                onClicked: {
                    if(!deleteSettingLine.inDeleteSetting){
                        showDelete.restart()
                        deleteSettingLine.inDeleteSetting = true
                    }
                }
            }
        }
        DSeparatorHorizontal {}
    }

    Item {
        id: buttonLine
        width: parent.width
        height: 38
        property int rightPadding: 15

        Row {
            id: saveBox
            spacing: 6
            height: parent.height
            x: parent.x + parent.width - width
            visible: opacity > 0

            DTextButton {
                anchors.verticalCenter: parent.verticalCenter
                text: dsTr("Save")
                onClicked: {
                    checkAllKeys()
                    if (connectionSessionObject.Save()) {
                        stackView.reset()
                    }
                }
            }

            DTextButton {
                anchors.verticalCenter: parent.verticalCenter
                text: dsTr("Close")
                onClicked: {
                    connectionSessionObject.Close()
                    stackView.reset()
                }
            }

            Item {
                height: parent.height
                width: buttonLine.rightPadding
            }
        }

        Row {
            id: deleteBox
            spacing: 6
            height: parent.height
            anchors.left: saveBox.right

            DTextButton {
                anchors.verticalCenter: parent.verticalCenter
                text: dsTr("Delete")
                onClicked: {
                    if(uuid){
                        dbusNetwork.DeleteConnection(uuid)
                    }
                    connectionSessionObject.Close()
                    stackView.reset()
                }
            }

            DTextButton {
                anchors.verticalCenter: parent.verticalCenter
                text: dsTr("Cancel")
                onClicked: {
                    //connectionSessionObject.Close()
                    //stackView.reset()
                    cancelDelete.restart()
                    deleteSettingLine.inDeleteSetting = false
                }
            }
            Item {
                height: parent.height
                width: buttonLine.rightPadding
            }
        }
    }

}
