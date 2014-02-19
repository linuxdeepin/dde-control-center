import QtQuick 2.0
import QtQml.Models 2.1
import Deepin.Widgets 1.0

Column {
    width: parent.width
    //height: addCustomShortcutBox.visible ? childrenRect.height : customKeybindingExpand.height

    Column {
        id: customKeybindingExpand
        width: parent.width

        property bool inDeleteStatus: false
        property string currentActionStateName: ""

        signal addCustomShortcutEmit

        property var keyBindingIds: {
            var obj = new Object()
            for(var i in keyBindings){
                obj[keyBindings[i][0]] = keyBindings[i][1]
            }
            return obj
        }

        property url iconPath: {
            for(var i in conflictInvalid){
                if(typeof keyBindingIds[conflictInvalid[i]] != "undefined"){
                    return "images/error_key.png"
                }
            }
            for(var i in conflictValid){
                if(typeof keyBindingIds[conflictValid[i]] != "undefined"){
                    return "images/conflict_key.png"
                }
            }
            return ""
        }

        DBaseLine {
            property bool active: false

            leftLoader.sourceComponent: Row {
                anchors.left: parent.left
                anchors.leftMargin: 5
                spacing: 3
                Image {
                    source: customKeybindingExpand.iconPath
                    visible: customKeybindingExpand.iconPath != ""
                }

                DssH2 {
                    text: name
                }
            }

            rightLoader.sourceComponent: StateButtons {
                onCurrentActionStateNameChanged: {
                    if(customKeybindingExpand.currentActionStateName == "addButton"){
                        customKeybindingExpand.addCustomShortcutEmit()
                    }
                    customKeybindingExpand.currentActionStateName = currentActionStateName
                }
            }

        }

        DSeparatorHorizontal {}
            
        Rectangle {
            color: dconstants.contentBgColor
            width: parent.width
            height: childrenRect.height
            visible: customKeybindingExpand.currentActionStateName != "addButton"

            ListView {
                id: lists
                focus: true
                width: parent.width
                height: lists.count * 30
                model: keyBindings.length
                property var keyData: keyBindings

                delegate: ShortcutInput {
                    showDelete: customKeybindingExpand.currentActionStateName == "deleteButton"
                    info: keyBindings[index]
                    warning: {
                        for(var i in conflictValid){
                            if (info[0] == conflictValid[i]){
                                return "conflict"
                            }
                        }
                        for(var i in conflictInvalid){
                            if (info[0] == conflictInvalid[i]){
                                return "error"
                            }
                        }
                        return ""
                    }
                }
            }
        }

        Column {
            id: addCustomShortcutBox
            width: parent.width
            height: childrenRect.height
            property int spacing: 14
            property int inputTextWidth: 180
            visible: customKeybindingExpand.currentActionStateName == "addButton"

            Connections {
                target: customKeybindingExpand
                onAddCustomShortcutEmit: {
                    if(addCustomShortcutNameArea.name && addCustomShortcutCommandArea.command){
                        bindManagerId.AddKeyBind(addCustomShortcutNameArea.name, addCustomShortcutCommandArea.command, addCustomShortcutGrabArea.grabKeyCombination)
                    }
                }
            }

            DBaseLine{
                id: addCustomShortcutNameArea
                property string name: rightLoader.item.name
                color: dconstants.contentBgColor
                rightLoader.sourceComponent: Row {
                    spacing: addCustomShortcutBox.spacing
                    property alias name: nameInput.text

                    DssH3{
                        anchors.verticalCenter: parent.verticalCenter
                        text: "Name"
                    }

                    DTextInput {
                        id: nameInput
                        anchors.verticalCenter: parent.verticalCenter
                        width: addCustomShortcutBox.inputTextWidth
                    }
                }
            }

            DBaseLine{
                id: addCustomShortcutCommandArea
                property string command: rightLoader.item.command
                color: dconstants.contentBgColor
                rightLoader.sourceComponent: Row {
                    spacing: addCustomShortcutBox.spacing
                    property alias command: commandInput.text

                    DssH3{
                        anchors.verticalCenter: parent.verticalCenter
                        text: "Command"
                    }

                    DTextInput {
                        id: commandInput
                        anchors.verticalCenter: parent.verticalCenter
                        width: addCustomShortcutBox.inputTextWidth
                    }
                }
            }

            DBaseLine{
                id: addCustomShortcutGrabArea
                property string grabKeyCombination: ""
                color: dconstants.contentBgColor
                rightLoader.sourceComponent: Row {
                    spacing: addCustomShortcutBox.spacing

                    DssH3{
                        anchors.verticalCenter: parent.verticalCenter
                        text: "Shortcut"
                    }

                    Item {
                        id: addCustomShortcutTextInput
                        width: addCustomShortcutBox.inputTextWidth
                        height: 26
                        anchors.verticalCenter: parent.verticalCenter

                        property bool grabFlag: false

                        Rectangle {
                            width: addCustomShortcutBox.inputTextWidth
                            height: parent.height
                            radius: 4
                            color: "#101010"
                            visible: addCustomShortcutTextInput.grabFlag
                        }

                        DLabel {
                            id: field
                            font.pixelSize: 11
                            color: dconstants.fgDarkColor
                            anchors.left: parent.left
                            anchors.leftMargin: 4
                            text: dsTr("Please input new shortcuts")
                            anchors.verticalCenter: parent.verticalCenter
                            visible: addCustomShortcutTextInput.grabFlag
                        }

                        DLabel {
                            id: realShortcutName
                            font.pixelSize: 11
                            text: dsTr("Grab key combination")
                            anchors.verticalCenter: parent.verticalCenter
                            visible: !addCustomShortcutTextInput.grabFlag
                        }

                        Connections {
                            target: grabManagerId
                            onKeyReleaseEvent: {
                                if (customKeybindingExpand.currentActionStateName == "addButton"){
                                    addCustomShortcutTextInput.grabFlag = false
                                    print("Release:", arg0)
                                    if( arg0 == 'escape' | !arg0 ){
                                        realShortcutName.text = dsTr("Grab key combination")
                                    }
                                    else if( arg0=="backspace" ){
                                        realShortcutName.text = dsTr("Disable")
                                        addCustomShortcutGrabArea.grabKeyCombination = ""
                                    }
                                    else {
                                        realShortcutName.text = windowView.toHumanShortcutLabel(arg0)
                                        addCustomShortcutGrabArea.grabKeyCombination = arg0
                                    }
                                }
                            }
                        }

                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                addCustomShortcutTextInput.grabFlag = true
                                grabManagerId.GrabKeyboard()
                            }
                        }
                    } // End of grab key ShortcutInput
                }
            }

        } // End of addCustomShortcutBox

        DSeparatorHorizontal {}
    }
}
