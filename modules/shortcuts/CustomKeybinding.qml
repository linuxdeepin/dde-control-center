import QtQuick 2.0
import Deepin.Widgets 1.0

Column {
    id: customKeybindingExpand
    width: parent.width

    property int listMaxHeight

    property bool inDeleteStatus: false
    property string currentActionStateName: ""

    DBaseLine {
        id: customTitleLine
        leftLoader.sourceComponent: DssH2 {
            text: name
        }

        rightLoader.visible: !stopSetKeyBinding
        rightLoader.sourceComponent: StateButtons {
            deleteButton.visible: keyBindings.length > 0
            onCurrentActionStateNameChanged: {
                if(customKeybindingExpand.currentActionStateName == "addButton"){
                    addCustomShortcutBox.height = 0
                }
                customKeybindingExpand.currentActionStateName = currentActionStateName
                if(currentActionStateName == "addButton"){
                    newShortcutName.content.item.text = ""
                    newShortcutCommand.content.item.text = ""
                    addCustomShortcutBox.height = addCustomShortcutBox.realHeight
                }
            }
        }
    }

    DSeparatorHorizontal {}

    Rectangle {
        id: customKeybindingContentArea
        color: dconstants.contentBgColor
        width: parent.width
        height: childrenRect.height
        clip: true

        Column {
            id: customKeybindingListBox
            width: parent.width
            height: customKeybindingList.count > 0 ? realHeight : 0

            property int realHeight: childrenRect.height

            ListView {
                id: customKeybindingList
                width: parent.width
                height: childrenRect.height
                model: keyBindings.length
                clip: true

                onCountChanged: {
                    if(count == 0 && customTitleLine.rightLoader.item){
                        customTitleLine.rightLoader.item.currentActionStateName = ""
                    }
                }

                delegate: ShortcutInput {
                    showDelete: customKeybindingExpand.currentActionStateName == "deleteButton"
                    info: keyBindings[index]
                }
            }

            DSeparatorHorizontal {
                visible: customKeybindingList.count > 0
            }
        }

        Column {
            id: addCustomShortcutBox
            width: parent.width
            height: 0
            anchors.top: customKeybindingListBox.bottom
            clip: true
            property int inputTextWidth: 180

            property int realHeight: childrenRect.height

            Behavior on height {
                PropertyAnimation { duration: 100 }
            }

            property int leftWidth: Math.max(newShortcutName.title.contentWidth, newShortcutCommand.title.contentWidth) + 24
            property int rightWidth: width - leftWidth - 24

            DCenterLine {
                id: newShortcutName
                title.text: dsTr("Name")
                leftWidth: addCustomShortcutBox.leftWidth
                content.sourceComponent: DTextInput{
                    activeFocusOnTab: true
                    width: addCustomShortcutBox.rightWidth
                }
            }

            DCenterLine {
                id: newShortcutCommand
                title.text: dsTr("Command")
                leftWidth: addCustomShortcutBox.leftWidth
                content.sourceComponent: DTextInput{
                    activeFocusOnTab: true
                    width: addCustomShortcutBox.rightWidth
                }
            }

            DBaseLine{
                id: addCustomShortcutGrabArea
                rightLoader.sourceComponent: Row {
                    spacing: 6
                    DTextButton {
                        text: dsTr("Cancel")
                        onClicked: {
                            customTitleLine.rightLoader.item.currentActionStateName = ""
                        }
                    }
                    DTextButton {
                        text: dsTr("Add")
                        onClicked: {
                            customTitleLine.rightLoader.item.currentActionStateName = ""
                            bindManagerId.AddKeyBind(newShortcutName.content.item.text, newShortcutCommand.content.item.text, "")
                        }
                    }
                }
            }

            DSeparatorHorizontal {}
        } // End of addCustomShortcutBox
    }

}
