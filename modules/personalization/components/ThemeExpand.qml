import QtQuick 2.1
import Deepin.Widgets 1.0
import "../widgets"

MyBaseExpand {
    id: themes_expand

    content.sourceComponent: Item {
        width: personalizationModule.width
        height: themeView.height

        GridView {
            id: themeView
            anchors.horizontalCenter: parent.horizontalCenter
            width: parent.width - 22
            height: Math.min(childrenRect.height, contentArea.maxConetentHeight)

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

            delegate: ThemeItem {
                width: cellWidth
                height: cellHeight
                selectedItemValue: themeView.currentItemName

                onSelectAction: {
                    themeView.selectItem(itemValue)
                }

                onPreviewAction:{
                    previewsWindow.showWindow(themeObj)
                }
            }
        } // end of GridView
    }
}
