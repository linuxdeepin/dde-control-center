import QtQuick 2.1
import "../widgets"

GridView {
    id: avatar_icon_view
    clip: true
    
    cellWidth: 103
    cellHeight: 93

    property bool withAddButton: false

    signal initialize
    signal avatarSet (Item item)

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
                    avatar_icon_view.avatarSet(round_image)
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
