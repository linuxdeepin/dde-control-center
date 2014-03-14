import QtQuick 2.1
import DBus.Com.Deepin.Daemon.Themes 1.0
import Deepin.Widgets 1.0

Item {
    id: personalization
    anchors.fill: parent

    property var constants: DConstants {}
    property var listModelComponent: DListModelComponent {}
    property url dataDir: windowView.getModuleDataDir("personalization")

    property int activeExpandIndex: 0

    property var dbusThemeManager: ThemeManager{}
    property var dbusThumbPath: ThumbPath{ path: "/com/deepin/daemon/ThemeManager" }

    Component{
        id: themeComponent
        Theme {}
    }

    Component.onCompleted: {
    }
    
    Column {
        anchors.top: parent.top
        width: parent.width

        DssTitle {
            text: dsTr("Personalization")
        }

        DSeparatorHorizontal {}

        DBaseExpand {
            id: themes_expand
            property int myIndex: 0
            expanded: activeExpandIndex == myIndex
            onExpandedChanged: {
                if(header.item){
                    header.item.active = expanded
                }
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
                Component.onCompleted: {
                    active = (activeExpandIndex == root.myIndex)
                }
            }

            content.sourceComponent: Item {
                width: personalization.width
                height: themeList.height

                ThemeView {
                    id: themeList
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: parent.width - 22
                    height: childrenRect.height

                    model: {
                        var myModel = listModelComponent.createObject(themeList, {})
                        var themeList = dbusThemeManager.themeList
                        for(var i in themeList){
                            var themeObj = themeComponent.createObject(personalization, { path: themeList[i] })
                            myModel.append({
                                "item_img_url": themeObj.thumbnailPath,
                                "item_name": themeObj.name,
                                "item_value": themeObj.name,
                                "themeObj": themeObj
                            })
                        }
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
                width: personalization.width
                height: childrenRect.height

                ThemeView {
                    id: gtkList
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: parent.width - 22
                    height: childrenRect.height

                    model: {
                        var myModel = listModelComponent.createObject(gtkList, {})
                        var themes = dbusThemeManager.gtkThemeList
                        for(var i in themes){
                            var theme_id = themes[i]
                            myModel.append({
                                "item_img_url": dbusThumbPath.GtkPath(theme_id),
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
                width: personalization.width
                height: childrenRect.height

                ThemeView {
                    id: cursorList
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: parent.width - 22
                    height: childrenRect.height

                    model: {
                        var myModel = listModelComponent.createObject(cursorList, {})
                        var themes = dbusThemeManager.cursorThemeList
                        for(var i in themes){
                            var theme_id = themes[i]
                            myModel.append({
                                "item_img_url": dbusThumbPath.CursorPath(theme_id),
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
                width: personalization.width
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
                width: personalization.width
                height: childrenRect.height
                
                ThemeView {
                    id: iconList
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: parent.width - 22
                    height: childrenRect.height

                    model: {
                        var myModel = listModelComponent.createObject(iconList, {})
                        var themes = dbusThemeManager.iconThemeList
                        for(var i in themes){
                            var theme_id = themes[i]
                            myModel.append({
                                "item_img_url": dataDir + "/icons/" + theme_id + "/thumbnail.png",
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
                width: personalization.width
                height: childrenRect.height
                
            }
        }

        DSeparatorHorizontal {}
    }
}
