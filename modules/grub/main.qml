import QtQuick 2.1
import Deepin.Widgets 1.0

Rectangle {
    id: root
    color: "#1A1B1B"
    width: 310
    height: 600

    Column {
        id: contentColumn

        DssTitle { text: "Grub2"}

        Rectangle {
            color: "transparent"
            width: 310
            height: 1
        }
        DSeparatorHorizontal{}        
        Preview { anchors.horizontalCenter: parent.horizontalCenter}
        DSeparatorHorizontal{}        

        DBaseExpand {
            id: delay_expand
            expanded: true
            header.sourceComponent: DBaseLine {
                leftLoader.sourceComponent: DssH2 {
                    text: "Delay"
                }
            }
            content.sourceComponent: Rectangle {
                width: 310
                height: 30
            }
        }

        DSeparatorHorizontal{}

        DBaseExpand {
            id: normal_item_expand

            header.sourceComponent: DDownArrowHeader {
                text: "Item Color"
                onClicked: {
                    normal_item_expand.expanded = !normal_item_expand.expanded
                }
            }
            content.sourceComponent: ColorPicker{
                width: root.width
                height: 220
            }
        }

        DSeparatorHorizontal{}

        DBaseExpand {
            id: selected_item_expand

            header.sourceComponent: DDownArrowHeader {
                text: "Selected Item Color"
                onClicked: {
                    selected_item_expand.expanded = !selected_item_expand.expanded
                }
            }
            content.sourceComponent: ColorPicker{
                width: root.width
                height: 220
            }
        }

        DSeparatorHorizontal{}

        DBaseExpand {
            id: theme_expand

            header.sourceComponent: DDownArrowHeader {
                text: "Theme"
                onClicked: {
                    theme_expand.expanded = !theme_expand.expanded
                }
            }
            content.sourceComponent: Rectangle {
                width: 310
                height: 30
            }
        }
    }
}