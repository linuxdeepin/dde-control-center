import QtQuick 2.1
import DBus.Com.Deepin.Daemon.Themes 1.0
import Deepin.Widgets 1.0

Item {
    id: personalizationModule
    anchors.fill: parent

    property int cellWidth: 144
    property int cellHeight: 112

    property var constants: DConstants {}
    property var listModelComponent: DListModelComponent {}

    property int activeExpandIndex: 0

    property var dbusThemeManager: ThemeManager{}
    property var dbusThumbPath: ThumbPath{ path: "/com/deepin/daemon/ThemeManager" }
    property var dbusPreviewPath: PreviewPath{ path: "/com/deepin/daemon/ThemeManager" }
    property var currentThemeObject: {
        var path = dbusThemeManager.GetPathViaName(dbusThemeManager.currentTheme)
        var obj = themeComponent.createObject(personalizationModule, { path: path[0] })
        return obj
    }

    property var previewsWindow: PreviewWindow{
        x: rootWindow.x - width
        y: rootWindow.y + 18
        themeObject: currentThemeObject
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
                width: personalizationModule.width
                height: themeView.height

                GridView {
                    id: themeView
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: parent.width - 22
                    height: childrenRect.height

                    cellWidth: personalizationModule.cellWidth
                    cellHeight: personalizationModule.cellHeight

                    property string currentItemName: dbusThemeManager.currentTheme

                    function selectItem(itemValue){
                        dbusThemeManager.SetCurrentTheme(itemValue)
                    }

                    Connections{
                        target: dbusThemeManager
                        onThemeListChanged: {
                            themeView.load_theme_list()
                        }
                    }

                    function load_theme_list(){
                        var themeList = dbusThemeManager.themeList
                        themeView.model.clear()
                        for(var i in themeList){
                            var themeObj = themeComponent.createObject(personalizationModule, { path: themeList[i] })

                            if(themeObj.name == "Custom"){
                                var thumbnailPath = themeObj.backgroundFile
                            }
                            else{
                                var thumbnailPath = themeObj.thumbnailPath
                            }

                            themeView.model.append({
                                "item_img_url": thumbnailPath,
                                "item_name": themeObj.name,
                                "item_value": themeObj.name,
                                "themeObj": themeObj
                            })
                        }
                    }

                    Component.onCompleted: load_theme_list()

                    model: ListModel{}

                    delegate: ThemeItem{
                        width: cellWidth
                        height: cellHeight
                        selectedItemValue: themeView.currentItemName

                        onSelectAction: {
                            themeView.selectItem(itemValue)
                        }

                        onPreviewAction:{
                            if (themeObj.name == "Custom"){
                                //previewsWindow.hideWindow()
                            }
                            else {
                                previewsWindow.showWindow(themeObj)
                            }
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
                width: personalizationModule.width
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
                width: personalizationModule.width
                height: childrenRect.height

                ComponentThemeView {
                    id: cursorList
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: parent.width - 22
                    height: childrenRect.height

                    property string currentItemName: currentThemeObject.cursorTheme

                    function selectItem(itemValue){
                        dbusThemeManager.SetCursorTheme(itemValue)
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
                width: personalizationModule.width
                height: childrenRect.height

                GridView {
                    id: wallpapperView
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: parent.width - 22
                    height: childrenRect.height

                    cellWidth: personalizationModule.cellWidth
                    cellHeight: personalizationModule.cellHeight - 14

                    property string currentItemName: currentThemeObject.backgroundFile

                    function selectItem(itemValue){
                        dbusThemeManager.SetBackgroundFile(itemValue)
                    }

                    model: dbusThemeManager.backgroundList

                    delegate: WallpapperItem {
                        width: cellWidth
                        height: cellHeight - 14
                        selectedItemValue: wallpapperView.currentItemName

                        onSelectAction: {
                            wallpapperView.selectItem(itemValue)
                        }

                    }

                }
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
                width: personalizationModule.width
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

            content.sourceComponent:  DCenterLine {
                height: 50
                color: dconstants.contentBgColor
                centerPadding: 20
                leftWidth: 70
                title.text: dsTr("Size")
                content.sourceComponent: DSliderEnhanced {
                    width: 190

                    min: 9
                    max: 16
                    init: 11

                    onValueConfirmed:{
                    }

                    Component.onCompleted: {
                        //addRuler(9, "")
                        //addRuler(10, "")
                        //addRuler(11, "")
                        //addRuler(12, "")
                        //addRuler(13, "")
                        //addRuler(14, "")
                        //addRuler(15, "")
                        //addRuler(16, "")
                    }
                }
            }
        }

        DSeparatorHorizontal {}
    }
}
