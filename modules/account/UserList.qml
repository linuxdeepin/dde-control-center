import QtQuick 2.1
import "../widgets"
/* import DBus.Org.Freedesktop.Accounts 1.0 */

ListView {
    id: root

    property int leftPadding: 15
    property int rightPadding: 15
    property int avatarNamePadding: 30
    
    /* property variant dbus_accounts: Accounts {} */

    Component {
        id: delegate_component

        Rectangle{
            id: component_bg
            color: "#1A1B1B"
            state: "action"

            width: 310
            height: component_top.height + component_sep.height
            
            Rectangle {
                id: component_top

                height: 100
                color: Qt.rgba(0, 0, 0, 0)

                DRoundImage {
                    id: round_image
                    roundRadius: 25
                    imageSource: userAvatar
                    
                    property bool toggleFlag: false
                    
                    onClicked: {
                        if (!toggleFlag) {
                            component_bg.state = "edit_dialog"
                        } else {
                            component_bg.state = "normal"
                        }
                        toggleFlag = !toggleFlag
                    }
                    
                    anchors.verticalCenter: parent.verticalCenter
                }
                
                Column {
                    id: name_column
                    
                    DssH2 {
                        text: userName
                    }
                    
                    DssH3 {
                        text: userType
                    }
                    
                    anchors.left: round_image.right
                    anchors.leftMargin: root.avatarNamePadding
                    anchors.verticalCenter: parent.verticalCenter
                }
                
                UserStatusButton {
                    id: user_status_button
                    state: userStatus
                    
                    anchors.right: parent.right
                    anchors.rightMargin: root.rightPadding
                    anchors.verticalCenter: parent.verticalCenter
                }
                
                DeleteUserButton {
                    id: delete_user_button
                    
                    onClicked: {
                        component_bg.state = "delete_dialog"
                    }
                    
                    anchors.right: parent.right
                    anchors.rightMargin: root.rightPadding
                    anchors.verticalCenter: parent.verticalCenter
                }
                
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.leftMargin: root.leftPadding
                anchors.rightMargin: root.rightPadding
            }

            DSeparatorHorizontal {
                id: component_sep
                
                anchors.top: component_top.bottom
            }
            
            DeleteUserDialog {
                id: delete_user_dialog
                width: 310
                height: 100
                visible: false
                
                onCancel: {
                    component_bg.state = "action"
                }
                
                onConfirm: {
                    component_bg.state = "normal"                    
                    print("delete user files? ", deleteFiles)
                }
                
                anchors.top: component_sep.bottom
            }
            
            EditUserDialog {
                id: edit_user_dialog
                
                anchors.top: component_sep.bottom
            }
            
            states: [
                State {
                    name: "normal"
                    PropertyChanges { 
                        target: delete_user_dialog
                        visible: false
                    }
                    PropertyChanges { 
                        target: edit_user_dialog
                        visible: false
                    }
                    PropertyChanges { 
                        target: component_bg
                        height: component_top.height + component_sep.height
                    }
                    PropertyChanges { 
                        target: user_status_button
                        visible: true
                    }
                    PropertyChanges { 
                        target: delete_user_button
                        visible: false
                    }
                },
                State {
                    name: "action"
                    PropertyChanges { 
                        target: delete_user_dialog
                        visible: false
                    }
                    PropertyChanges { 
                        target: edit_user_dialog
                        visible: false
                    }
                    PropertyChanges { 
                        target: component_bg
                        height: component_top.height + component_sep.height
                    }
                    PropertyChanges { 
                        target: user_status_button
                        visible: false
                    }
                    PropertyChanges { 
                        target: delete_user_button
                        visible: true
                    }
                },
                State {
                    name: "delete_dialog"
                    PropertyChanges { 
                        target: delete_user_dialog
                        visible: true
                    }                    
                    PropertyChanges { 
                        target: edit_user_dialog
                        visible: false
                    }                    
                    PropertyChanges { 
                        target: component_bg
                        height: component_top.height + component_sep.height + delete_user_dialog.height
                    }
                    PropertyChanges { 
                        target: user_status_button
                        visible: false
                    }
                    PropertyChanges { 
                        target: delete_user_button
                        visible: true
                    }
                },
                State {
                    name: "edit_dialog"
                    PropertyChanges { 
                        target: delete_user_dialog
                        visible: false
                    }                    
                    PropertyChanges { 
                        target: edit_user_dialog
                        visible: true
                    }                    
                    PropertyChanges { 
                        target: component_bg
                        height: component_top.height + component_sep.height + edit_user_dialog.height
                    }
                    PropertyChanges { 
                        target: user_status_button
                        visible: false
                    }
                    PropertyChanges { 
                        target: delete_user_button
                        visible: false
                    }
                }
            ]
            
            Behavior on height {
                SmoothedAnimation { duration: 200 }
            }
            
            Component.onCompleted: {
                if (index == 0) {root.width = width}
                root.height += height
            }
        }
    }

    model: ListModel { id: user_list_model        
        ListElement {
            userAvatar: "/home/hualet/Pictures/DeepinScreenshot20131108122543.png"
            userName: "Hualet0"
            userType: "Administrator"
            userStatus: "currentUser"
        }
        ListElement {
            userAvatar: "/home/hualet/Pictures/DeepinScreenshot20131108122543.png"
            userName: "Hualet1"
            userType: "Administrator"
            userIsCurrent: "false"
            userStatus: "otherUser"            
        }
        ListElement {
            userAvatar: "/home/hualet/Pictures/DeepinScreenshot20131108122543.png"
            userName: "Hualet2"
            userType: "Administrator"
            userStatus: "inactiveUser"            
        }
    }

    delegate: delegate_component
    
    Component.onCompleted: {
        print(dbus_accounts.ListCachedUsers())
    }
}