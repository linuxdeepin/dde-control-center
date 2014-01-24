import QtQuick 2.1
import Deepin.Widgets 1.0

Rectangle {
    color: "#1A1B1B"
    /* anchors.fill: parent */
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

        DBaseExpand {
            id: delay_expand

            header.sourceComponent: DSwitchButtonHeader {
                id: delay_switch_button
                text: "Delay"

                onActiveChanged: {
                    delay_expand.expanded = active
                }
            }
            content.sourceComponent: Rectangle {
                width: 310
                height: 30
            }
        }

        DSeparatorHorizontal{}

        DBaseExpand {
            id: resolution_expand

            header.sourceComponent: DDownArrowHeader {
                text: "Resolution"
                onClicked: {
                    resolution_expand.expanded = !resolution_expand.expanded
                }
            }
            content.sourceComponent: Rectangle {
                width: 310
                height: 30
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