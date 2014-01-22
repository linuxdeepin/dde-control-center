import QtQuick 2.1
import DBus.Com.Deepin.Daemon.Individuation 1.0
import Deepin.Widgets 1.0

Item {
    id: individuation
    anchors.fill: parent

    property var constants: DConstants {}
    property var dbus_individuation: Individuation {}
    
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
                    id: theme_view
                    width: parent.width - 2 * 15
                    height: parent.height - 2 * 15
                    anchors.centerIn: parent

                    Component.onCompleted: {
                        var avaThemes = dbus_individuation.availableBackground
                        for(var i = 0; i < avaThemes.length; i++) {
                            model.append({themeId: avaThemes[i][0]})
                        }
                    }
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
                height: window_themes_select.height
                
                SelectView {
                    id: window_themes_select
                    width: parent.width - 15 * 2
                    anchors.horizontalCenter: parent.horizontalCenter
                    
                    Component.onCompleted: {
                        var avaThemes = dbus_individuation.availableGtkTheme
                        for(var i = 0; i < avaThemes.length; i++) {
                            model.append({itemId: avaThemes[i][0], itemText: avaThemes[i][0], itemWidth: 0})
                        }
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
                height: cursor_themes_select.height
                
                SelectView {
                    id: cursor_themes_select
                    width: parent.width - 15 * 2
                    anchors.horizontalCenter: parent.horizontalCenter
                    
                    Component.onCompleted: {
                        var avaThemes = dbus_individuation.availableCursorTheme
                        for(var i = 0; i < avaThemes.length; i++) {
                            model.append({itemId: avaThemes[i][0], itemText: avaThemes[i][0], itemWidth: 0})
                        }
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
                height: wallpaper_themes_select.height
                
                SelectView {
                    id: wallpaper_themes_select
                    width: parent.width - 15 * 2
                    anchors.horizontalCenter: parent.horizontalCenter
                    
                    Component.onCompleted: {
                        var avaThemes = dbus_individuation.availableBackground
                        for(var i = 0; i < avaThemes.length; i++) {
                            model.append({itemId: avaThemes[i][0], itemText: avaThemes[i][0], itemWidth: 0})
                        }
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
                height: icons_themes_select.height
                
                SelectView {
                    id: icons_themes_select
                    width: parent.width - 15 * 2
                    anchors.horizontalCenter: parent.horizontalCenter
                    
                    Component.onCompleted: {
                        var avaThemes = dbus_individuation.availableIconTheme
                        for(var i = 0; i < avaThemes.length; i++) {
                            model.append({itemId: avaThemes[i][0], itemText: avaThemes[i][0], itemWidth: 0})
                        }
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
                height: font_themes_select.height
                
                SelectView {
                    id: font_themes_select
                    width: parent.width - 15 * 2
                    anchors.horizontalCenter: parent.horizontalCenter
                    
                    Component.onCompleted: {
                        var avaThemes = dbus_individuation.availableFontTheme
                        for(var i = 0; i < avaThemes.length; i++) {
                            model.append({itemId: avaThemes[i][0], itemText: avaThemes[i][0], itemWidth: 0})
                        }
                    }
                }
            }
        }
    }
}