import QtQuick 2.1
import Deepin.Widgets 1.0

GridView {
    id: avatar_icon_view
    clip: true

    cellWidth: 103
    cellHeight: 93

    property bool withAddButton: false

    signal initialize
    signal avatarSet (url path)
    signal checkPrivate (int idx)

    Component {
        id: avatar_icon_view_delegate

        Item {
            width: GridView.view.cellWidth
            height: GridView.view.cellHeight

            Connections {
                target: avatar_icon_view

                onCheckPrivate: {
                    if (index == idx) {
                        round_image.state = "checked"
                        avatar_icon_view.avatarSet(round_image.imageSource)
                    } else {
                        round_image.state = "normal"
                    }
                }
            }

            DRoundImage {
                id: round_image

                roundRadius: 30
                borderWidth: 2
                glowRadius: 2
                imageSource: avatarPath
                checkSupport: !(withAddButton && index == (count - 1))

                onClicked: {
                    if (checkSupport) {
                        avatar_icon_view.checkPrivate(index)
                    }
                }

                anchors.centerIn: parent
            }
        }
    }

    delegate: avatar_icon_view_delegate
    model: ListModel { id: avatar_icon_view_model }

    Component.onCompleted: {
        avatar_icon_view.initialize()

        if (withAddButton) {
            avatar_icon_view_model.append({"avatarPath": "images/avatar_add.png"})
        }

        width = 310
        height = cellHeight * Math.min(3, Math.ceil(count /3))
    }
}
