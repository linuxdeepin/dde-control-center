import QtQuick 2.1
import "../widgets"
import DBus.Org.Freedesktop.Accounts 1.0

ListView {
    id: root

    property int leftPadding: 15
    property int rightPadding: 15
    property int avatarNamePadding: 30
    
    property variant dbus_accounts: Accounts {}

    Component {
        id: delegate_component

        Rectangle{
            id: component_bg
            color: "#191919"

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
        }
        ListElement {
            userAvatar: "/home/hualet/Pictures/DeepinScreenshot20131108122543.png"
            userName: "Hualet1"
            userType: "Administrator"
        }
        ListElement {
            userAvatar: "/home/hualet/Pictures/DeepinScreenshot20131108122543.png"
            userName: "Hualet2"
            userType: "Administrator"
        }
    }

    delegate: delegate_component
    
    Component.onCompleted: {
        print(dbus_accounts.ListCachedUsers())
    }
}