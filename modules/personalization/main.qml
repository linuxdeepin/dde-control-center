import QtQuick 2.1
import DBus.Com.Deepin.Daemon.Personalization 1.0
import Deepin.Widgets 1.0

Item {
    id: individuation
    anchors.fill: parent

    property var constants: DConstants {}
    property var dbusPersonalization: Personalization {}
    property var listModelComponent: DListModelComponent {}
    property url dataDir: windowView.getModuleDataDir("personalization")

    property int activeExpandIndex: 0
    
    Column {
        anchors.top: parent.top
        width: parent.width

        DssTitle {
            text: dsTr("Individuation")
        }

        DSeparatorHorizontal {}

        DBaseExpand {
            id: themes_expand
            property int myIndex: 0
            expanded: activeExpandIndex == myIndex
            onExpandedChanged: {
                header.item.active = expanded
            }

            header.sourceComponent: DDownArrowHeader {
                text: dsTr("Themes")
                onClicked: {
                    if(activeExpandIndex == root.myIndex){
                        activeExpandIndex = -1
                    }
                    else{
                        activeExpandIndex = root.myIndex
                    }
                }
            }

            content.sourceComponent: Item {
                width: individuation.width
                height: themeList.height

                ThemeView {
                    id: themeList
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: parent.width - 22
                    height: childrenRect.height

                    model: {
                        var myModel = listModelComponent.createObject(themeList, {})
                        myModel.append({
                            "item_img_url": "/themes/Deepin/thumbnail.png",
                            "item_name": "Deepin",
                            "item_value": "deepin"
                        })
                        myModel.append({
                            "item_img_url": "/themes/Deepin-Blue/thumbnail.png",
                            "item_name": "Deepin Blue",
                            "item_value": "deepin-blue"
                        })
                        return myModel
                    }
                }
            }
        }

        DSeparatorHorizontal {}

        DBaseExpand {
            id: window_themes_expand
            property int myIndex: 1
            expanded: activeExpandIndex == myIndex
            onExpandedChanged: {
                header.item.active = expanded
            }

            header.sourceComponent: DDownArrowHeader {
                text: dsTr("GTK Window")
                onClicked: {
                    if(activeExpandIndex == root.myIndex){
                        activeExpandIndex = -1
                    }
                    else{
                        activeExpandIndex = root.myIndex
                    }
                }
            }

            content.sourceComponent: Item {
                width: individuation.width
                height: childrenRect.height

                ThemeView {
                    id: gtkList
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: parent.width - 22
                    height: childrenRect.height

                    model: {
                        var myModel = listModelComponent.createObject(gtkList, {})
                        var themes = dbusPersonalization.availableGtkTheme
                        for(var i in themes){
                            var theme_id = themes[i][0]
                            myModel.append({
                                "item_img_url": "/gtk/" + theme_id + "/thumbnail.png",
                                "item_name": windowView.toHumanThemeName(theme_id),
                                "item_value": theme_id
                            })
                        }
                        return myModel
                    }
                }
                
            }
        }

        DSeparatorHorizontal {}

        DBaseExpand {
            id: cursor_themes_expand
            property int myIndex: 2
            expanded: activeExpandIndex == myIndex
            onExpandedChanged: {
                header.item.active = expanded
            }

            header.sourceComponent: DDownArrowHeader {
                text: dsTr("Cursor")
                onClicked: {
                    if(activeExpandIndex == root.myIndex){
                        activeExpandIndex = -1
                    }
                    else{
                        activeExpandIndex = root.myIndex
                    }
                }
            }

            content.sourceComponent: Item {
                width: individuation.width
                height: childrenRect.height

                ThemeView {
                    id: cursorList
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: parent.width - 22
                    height: childrenRect.height

                    model: {
                        var myModel = listModelComponent.createObject(cursorList, {})
                        var themes = dbusPersonalization.availableCursorTheme
                        for(var i in themes){
                            var theme_id = themes[i][0]
                            myModel.append({
                                "item_img_url": "/cursor/" + theme_id + "/thumbnail.png",
                                "item_name": windowView.toHumanThemeName(theme_id),
                                "item_value": theme_id
                            })
                        }
                        return myModel
                    }
                }
                
            }
        }

        DSeparatorHorizontal {}

        DBaseExpand {
            id: wallpaper_themes_expand
            property int myIndex: 3
            expanded: activeExpandIndex == myIndex
            onExpandedChanged: {
                header.item.active = expanded
            }

            header.sourceComponent: DDownArrowHeader {
                text: dsTr("Wallpaper")
                onClicked: {
                    if(activeExpandIndex == root.myIndex){
                        activeExpandIndex = -1
                    }
                    else{
                        activeExpandIndex = root.myIndex
                    }
                }
            }

            content.sourceComponent: Item {
                width: individuation.width
                height: childrenRect.height
                
            }
        }

        DSeparatorHorizontal {}

        DBaseExpand {
            id: icon_themes_expand
            property int myIndex: 4
            expanded: activeExpandIndex == myIndex
            onExpandedChanged: {
                header.item.active = expanded
            }

            header.sourceComponent: DDownArrowHeader {
                text: dsTr("Icons")
                onClicked: {
                    if(activeExpandIndex == root.myIndex){
                        activeExpandIndex = -1
                    }
                    else{
                        activeExpandIndex = root.myIndex
                    }
                }
            }

            content.sourceComponent: Item {
                width: individuation.width
                height: childrenRect.height
                
                ThemeView {
                    id: iconList
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: parent.width - 22
                    height: childrenRect.height

                    model: {
                        var myModel = listModelComponent.createObject(iconList, {})
                        var themes = dbusPersonalization.availableIconTheme
                        for(var i in themes){
                            var theme_id = themes[i][0]
                            myModel.append({
                                "item_img_url": "/icons/" + theme_id + "/thumbnail.png",
                                "item_name": windowView.toHumanThemeName(theme_id),
                                "item_value": theme_id
                            })
                        }
                        return myModel
                    }
                }

            }
        }

        DSeparatorHorizontal {}

        DBaseExpand {
            id: font_themes_expand
            property int myIndex: 5
            expanded: activeExpandIndex == myIndex
            onExpandedChanged: {
                header.item.active = expanded
            }

            header.sourceComponent: DDownArrowHeader {
                text: dsTr("Font")
                onClicked: {
                    if(activeExpandIndex == root.myIndex){
                        activeExpandIndex = -1
                    }
                    else{
                        activeExpandIndex = root.myIndex
                    }
                }
            }

            content.sourceComponent: Item {
                width: individuation.width
                height: childrenRect.height
                
            }
        }

        DSeparatorHorizontal {}
    }
}
