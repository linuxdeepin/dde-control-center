import QtQuick 2.1
import "../widgets"

DScrollWidget {
    id: root
    width: 300
    height: 300

    property bool withAddButton: false
    
    signal avatarSet (Item item)

    GridView {
        id: avatar_icon_view

        cellWidth: parent.width / 3
        cellHeight: parent.height / 3

        Component {
            id: avatar_icon_view_delegate

            Item {
                width: GridView.view.cellWidth
                height: GridView.view.cellHeight

                DRoundImage {
                    id: round_image
                    
                    roundRadius: 30
                    borderWidth: 2
                    glowRadius: 2
                    imageSource: avatarPath
                    checkSupport: true

                    onClicked: {
                        root.avatarSet(round_image)
                    }

                    anchors.centerIn: parent
                }

            }
        }

        delegate: avatar_icon_view_delegate
        model: ListModel { id: avatar_icon_view_model }

        Component.onCompleted: {
            avatar_icon_view_model.append([{"avatarPath": "/var/lib/AccountsService/icons/001.jpg"},
                                           {"avatarPath": "/var/lib/AccountsService/icons/002.jpg"},
                                           {"avatarPath": "/var/lib/AccountsService/icons/003.jpg"},
                                           {"avatarPath": "/var/lib/AccountsService/icons/004.jpg"},
                                           {"avatarPath": "/var/lib/AccountsService/icons/005.jpg"},
                                           {"avatarPath": "/var/lib/AccountsService/icons/006.jpg"},
                                           {"avatarPath": "/var/lib/AccountsService/icons/007.jpg"},
                                           {"avatarPath": "/var/lib/AccountsService/icons/008.jpg"}])
            if (withAddButton) {
                avatar_icon_view_model.append({"avatarPath": "images/avatar_add.png"})
            }
        }

        anchors.fill: parent
    }
}
