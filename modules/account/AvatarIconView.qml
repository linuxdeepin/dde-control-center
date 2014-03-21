import QtQuick 2.1
import Deepin.Widgets 1.0

GridView {
    id: avatar_icon_view
    clip: true

    cellWidth: 96
    cellHeight: 93

    property bool withAddButton: false

    signal initialize
    signal avatarSet (url path)
    signal checkPrivate (int idx)

    AvatarPickDialog {
        id: avatar_pick_dialog
        onAccepted: {
            avatarSet(fileUrl)
        }
    }

    MouseArea {
        z: -1
        anchors.fill: parent
        onClicked: { parent.forceActiveFocus() }
    }

    Keys.onUpPressed: { avatar_icon_view.moveCurrentIndexUp() }
    Keys.onDownPressed: { avatar_icon_view.moveCurrentIndexDown() }
    Keys.onLeftPressed: { avatar_icon_view.moveCurrentIndexLeft() }
    Keys.onRightPressed: { avatar_icon_view.moveCurrentIndexRight() }

    Component {
        id: avatar_icon_view_delegate


        Item {
            width: GridView.view.cellWidth
            height: GridView.view.cellHeight

            function checkSelect() {
                if (index == avatar_icon_view.currentIndex && index != avatar_icon_view.count - 1) {
                    round_image.state = "checked"
                    avatar_icon_view.avatarSet(round_image.imageSource)
                } else {
                    round_image.state = "normal"
                }
            }

            Connections {
                target: avatar_icon_view

                onCurrentIndexChanged: { checkSelect() }
            }

            Component.onCompleted: {
                if (avatarPath == this_user.iconFile) {
                    avatar_icon_view.currentIndex = index
                    round_image.state = "checked"
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
                        avatar_icon_view.currentIndex = index
                        avatar_icon_view.forceActiveFocus()
                    } else {
                        avatar_pick_dialog.open()
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

        width = 290
        height = cellHeight * Math.min(3, Math.ceil(count /3))
    }
}
