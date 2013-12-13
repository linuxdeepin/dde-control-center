import QtQuick 2.1
import "../widgets"

DScrollWidget {
    width: 300
    height: 100

    property bool withAddButton: false

    GridView {
        id: avatar_icon_view

        cellWidth: parent.width / 3
        cellHeight: 70

        Component {
            id: avatar_icon_view_delegate

            Item {
                width: GridView.view.cellWidth
                height: GridView.view.cellHeight

                DRoundImage {
                    roundRadius: 30
                    imageSource: avatarPath
                    checkSupport: true

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
    }
}
