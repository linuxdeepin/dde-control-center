import QtQuick 2.1
import DBus.Com.Deepin.Daemon.Themes 1.0
import Deepin.Widgets 1.0

Item {
    id: personalization
    anchors.fill: parent

    property var constants: DConstants {}
    property var listModelComponent: DListModelComponent {}

    property int activeExpandIndex: 0

    property var dbusThemeManager: ThemeManager{}
    property var dbusThumbPath: ThumbPath{ path: "/com/deepin/daemon/ThemeManager" }
    property var dbusPreviewPath: PreviewPath{ path: "/com/deepin/daemon/ThemeManager" }
    property var currentThemeObject: {
        var path = dbusThemeManager.GetPathViaName(dbusThemeManager.currentTheme)
        var obj = themeComponent.createObject(personalization, { path: path[0] })
        return obj
    }

    property var previewsPics: {
        var pics = new Array()

        var currentThemePreviewPath = currentThemeObject.previewPath
        if(currentThemePreviewPath){
            pics.push(currentThemePreviewPath)
        }

        var currenGtkTheme = currentThemeObject.gtkTheme
        var currenGtkThemePreviewPath = dbusPreviewPath.GtkPath(currenGtkTheme)
        print("Gtk:", currenGtkTheme)
        if(currenGtkThemePreviewPath){
            pics.push(currenGtkThemePreviewPath)
        }

        var currentIconTheme = currentThemeObject.iconTheme
        var currentIconThemePreviewPath = dbusPreviewPath.IconPath(currentIconTheme)
        print("Icon:", currentIconTheme)
        if(currentIconThemePreviewPath){
            pics.push(currentIconThemePreviewPath)
        }

        var currentCursorTheme = currentThemeObject.gtkCursorTheme
        var currentCursorThemePreviewPath = dbusPreviewPath.CursorPath(currentCursorTheme)
        print("Cursor:", currentCursorTheme)
        if(currentCursorThemePreviewPath){
            pics.push(currentCursorThemePreviewPath)
        }

        return pics
    }

    property var previewsWindow: PreviewWindow{
        previews: previewsPics
        x: rootWindow.x - width
        y: rootWindow.y
    }

    Component.onCompleted: {
    }

    Component{
        id: themeComponent
        Theme {}
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

                GridView {
                    id: themeList
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: parent.width - 22
                    height: childrenRect.height

                    cellWidth: 144
                    cellHeight: 112

                    property string currentItemName: dbusThemeManager.currentTheme

                    function selectItem(itemValue){
                        dbusThemeManager.currentTheme = itemValue
                    }

                    model: {
                        var myModel = listModelComponent.createObject(themeList, {})
                        var themeList = dbusThemeManager.themeList
                        for(var i in themeList){
                            var themeObj = themeComponent.createObject(personalization, { path: themeList[i] })

                            if(themeObj.name == "Custom"){
                                var thumbnailPath = themeObj.backgroundFile
                            }
                            else{
                                var thumbnailPath = themeObj.thumbnailPath
                            }

                            myModel.append({
                                "item_img_url": thumbnailPath,
                                "item_name": themeObj.name,
                                "item_value": themeObj.name,
                                "themeObj": themeObj
                            })
                        }
                        return myModel
                    }

                    delegate: ThemeItem{
                        width: themeList.cellWidth
                        height: themeList.cellHeight
                        selectedItemValue: themeList.currentItemName

                        onSelectAction: {
                            themeList.selectItem(itemValue)
                        }

                        onPreviewAction:{
                            previewsWindow.showWindow()
                        }
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

                ComponentThemeView {
                    id: gtkList
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: parent.width - 22
                    height: childrenRect.height

                    property string currentItemName: currentThemeObject.gtkTheme

                    function selectItem(itemValue){
                        dbusThemeManager.SetGtkTheme(itemValue)
                    }

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

                ComponentThemeView {
                    id: cursorList
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: parent.width - 22
                    height: childrenRect.height

                    property string currentItemName: currentThemeObject.gtkCursorTheme

                    function selectItem(itemValue){
                        dbusThemeManager.SetGtkCursorTheme(itemValue)
                    }

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
                
                ComponentThemeView {
                    id: iconList
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: parent.width - 22
                    height: childrenRect.height

                    property string currentItemName: currentThemeObject.iconTheme

                    function selectItem(itemValue){
                        dbusThemeManager.SetIconTheme(itemValue)
                    }

                    model: {
                        var myModel = listModelComponent.createObject(iconList, {})
                        var themes = dbusThemeManager.iconThemeList
                        for(var i in themes){
                            var theme_id = themes[i]
                            myModel.append({
                                "item_img_url": dbusThumbPath.IconPath(theme_id),
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
