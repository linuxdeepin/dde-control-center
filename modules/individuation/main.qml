import QtQuick 2.1
import Deepin.Widgets 1.0

Item {
    id: individuation
    anchors.fill: parent

    property variant constants: DConstants {}

    Column {
        anchors.top: parent.top
        width: parent.width

        DssTitle {
            text: dsTr("Individuation")
        }

        DSeparatorHorizontal {}

        DBaseExpand {
            id: themes_expand

            header.sourceComponent: DDownArrowHeader {
                text: dsTr("Themes")
                onClicked: {
                    themes_expand.expanded = !themes_expand.expanded
                }
            }

            content.sourceComponent: Item {
                width: individuation.width
                height: 260

                ThemeView {
                    width: parent.width - 2 * 15
                    height: parent.height - 2 * 15

                    anchors.centerIn: parent
                }
            }
        }
        DBaseExpand {
            id: window_themes_expand

            header.sourceComponent: DDownArrowHeader {
                text: dsTr("Window")
                onClicked: {
                    window_themes_expand.expanded = !window_themes_expand.expanded
                }
            }

            content.sourceComponent: Item {
                width: individuation.width
                height: 50
                SelectView {
                    anchors.fill: parent

                    Component.onCompleted: {
                        select("hello_world")
                    }
                }
            }
        }

        DBaseExpand {
            id: cursor_themes_expand

            header.sourceComponent: DDownArrowHeader {
                text: dsTr("Cursor")
                onClicked: {
                    cursor_themes_expand.expanded = !cursor_themes_expand.expanded
                }
            }

            content.sourceComponent: Item {
                width: individuation.width
                height: 50
                SelectView {
                    anchors.fill: parent

                    Component.onCompleted: {
                        select("hello_world")
                    }
                }
            }
        }

        DBaseExpand {
            id: wallpaper_themes_expand

            header.sourceComponent: DDownArrowHeader {
                text: dsTr("Wallpaper")
                onClicked: {
                    wallpaper_themes_expand.expanded = !wallpaper_themes_expand.expanded
                }
            }

            content.sourceComponent: Item {
                width: individuation.width
                height: 50
                SelectView {
                    anchors.fill: parent

                    Component.onCompleted: {
                        select("hello_world")
                    }
                }
            }
        }

        DBaseExpand {
            id: icon_themes_expand

            header.sourceComponent: DDownArrowHeader {
                text: dsTr("Icons")
                onClicked: {
                    icon_themes_expand.expanded = !icon_themes_expand.expanded
                }
            }

            content.sourceComponent: Item {
                width: individuation.width
                height: 50
                SelectView {
                    anchors.fill: parent

                    Component.onCompleted: {
                        select("hello_world")
                    }
                }
            }
        }

        DBaseExpand {
            id: font_themes_expand

            header.sourceComponent: DDownArrowHeader {
                text: dsTr("Font")
                onClicked: {
                    font_themes_expand.expanded = !font_themes_expand.expanded
                }
            }

            content.sourceComponent: Item {
                width: individuation.width
                height: 50
                SelectView {
                    anchors.fill: parent

                    Component.onCompleted: {
                        select("hello_world")
                    }
                }
            }
        }
    }
}